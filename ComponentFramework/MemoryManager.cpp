#include "MemoryManager.h"
#include "Debug.h"


// NEW & DELETE OVERRIDES 
void* operator new(std::size_t numBytes)
{
	MemoryMonitor::GetInstance().setMemoryUsage(MemoryMonitor::GetInstance().getMemoryUsage() + numBytes);
	MemoryMonitor::GetInstance().Print(numBytes, true);

	return MemoryManager::GetInstance().allocate(numBytes);
}

void operator delete(void* memoryLocation, std::size_t numBytes)
{

	MemoryMonitor::GetInstance().setMemoryUsage(MemoryMonitor::GetInstance().getMemoryUsage() - numBytes);
	MemoryMonitor::GetInstance().Print(numBytes, false);

	MemoryManager::GetInstance().deallocate(memoryLocation);

}

void* operator new[](std::size_t numBytes) {

	MemoryMonitor::GetInstance().setMemoryUsage(MemoryMonitor::GetInstance().getMemoryUsage() + numBytes);
	MemoryMonitor::GetInstance().Print(numBytes, true);

	size_t* size = (size_t*)MemoryManager::GetInstance().allocate(numBytes + sizeof(size_t));
	size[0] = numBytes;
	return size + 1;
}

void operator delete[](void* memoryLocation) {
	if (memoryLocation != nullptr)
	{
		size_t* numBytes = (size_t*)memoryLocation - 1;
		if (numBytes)
		{
			MemoryMonitor::GetInstance().setMemoryUsage(MemoryMonitor::GetInstance().getMemoryUsage() - numBytes[0]);
			MemoryMonitor::GetInstance().Print(numBytes[0], false);

			MemoryManager::GetInstance().deallocate(numBytes);
		}
	}
}


// MEMORY MONITOR
MemoryMonitor::MemoryMonitor() : currentMemoryUsage(0), maxMemoryUsage(0), enablePrintText(false) {}

MemoryMonitor::~MemoryMonitor()
{
	if (currentMemoryUsage > 0) printf("MEMORY LEAK : %6d BYTES\n", currentMemoryUsage);
	else printf("No Memory Leaks Detected\n");
}

MemoryMonitor& MemoryMonitor::GetInstance()
{
	static MemoryMonitor instance;
	return instance;
}

void MemoryMonitor::Print()
{
	// print the data if enablePrintText is true
	if(enablePrintText)
	printf("MEMORY USAGE : %6u, MAX MEMORY USAGE : %10u\n", currentMemoryUsage, maxMemoryUsage);
}

void MemoryMonitor::Print(std::size_t numBytes, bool isAllocating)
{
	// print the data if enablePrintText is true
	if (enablePrintText) {
		if (isAllocating)
		{
			printf("  ALLOCATED : %6u, MEMORY USAGE : %10u, MAX MEMORY USAGE : %10u\n", numBytes, currentMemoryUsage, maxMemoryUsage);
		}
		else
		{
			printf("DEALLOCATED : %6u, MEMORY USAGE : %10u, MAX MEMORY USAGE : %10u\n", numBytes, currentMemoryUsage, maxMemoryUsage);
		}
	}
}



// MEMORY MANAGER
MemoryManager::MemoryManager()
{
	for (int n = 0; n < blockCount; n++)
	{
		char* blockSetIterator = memoryBlocks[n];
		{
			for (int i = 0; i < segmentCount[n]; i++)
			{
				size_t index = i * segmentSize[n];
				blockSetIterator[index] = 'a';
			}
		}
	}
	printf("PREALLOCATED MEMORY\n");
}

MemoryManager::~MemoryManager()
{
	printf("DEALLOCATING MEMORY\n");
}

MemoryManager& MemoryManager::GetInstance()
{
		static MemoryManager instance;
		return instance;
}

void* MemoryManager::allocate(const size_t numBytes)
{
	int blockSelector = 0;
	char* blockSetIterator = memoryBlocks[0];

	bool foundUsableBlock = false;

	for (int i = 0; i < blockCount; i++)
	{
		if (numBytes < segmentSize[i])
		{
			blockSelector = i;
			blockSetIterator = memoryBlocks[i];
			foundUsableBlock = true;
			break;
		}
	}

	if (!foundUsableBlock)
	{
		// TO SOLVE: CHECK 'numBytes' AND INCREASE THE SIZE OF THE LARGEST MEMORY SEGMENT SO IT CAN HOLD 'numBytes'
		std::string errorMsg = "LARGEST MEMORY SLOT CONTAINS TOO LITTLE MEMORY! NEEDS TO BE OF SIZE: ";
		unsigned long long v = numBytes;
		if (v > MB)
		{
			v /= MB;
			errorMsg.append(std::to_string(v) + " MB");
		}
		else if (v > KB)
		{
			v /= KB;
			errorMsg.append(std::to_string(v) + " KB");
		}
		else
		{
			errorMsg.append(std::to_string(v));
		};

		Debug::FatalError(errorMsg, __FILE__, __LINE__);
		throw;
		return nullptr;
	}

	for (size_t i = 0; i < segmentCount[blockSelector]; i++)
	{
		size_t index = segmentSize[blockSelector] * i;
		// Check if there is a free memory segment
		if (blockSetIterator[index] == 'a')
		{
			// Set the memory segment to be occupied and return a pointer to the memory segment
			memset(blockSetIterator + (index), 'b', 1);
			memset(blockSetIterator + (index + 1), NULL, (segmentSize[blockSelector] - 1));
			return blockSetIterator + (index + 1);
		}
	}
	
	std::string errorMsg = "NOT ENOUGH MEMORY SLOTS OF SIZE: ";
	unsigned long long v = numBytes;
	if (v > MB)
	{
		v /= MB;
		errorMsg.append(std::to_string(v) + " MB");
	}
	else if (v > KB)
	{
		v /= KB;
		errorMsg.append(std::to_string(v) + " KB");
	}
	else
	{
		errorMsg.append(std::to_string(v));
	}
	Debug::FatalError(errorMsg, __FILE__, __LINE__);
	// TO SOLVE: CHECK 'numBytes' AND INCREASE THE AMOUNT OF MEMORY SEGMENTS OF THAT SIZE UNTIL... 
	// ...THERE ARE ENOUGH SLOTS FOR ALL DATA OF SIZE 'numBytes'
	throw;
	return nullptr;
}

void MemoryManager::deallocate(void* memoryLocation)
{
	char* inUseByte = (char*)(memoryLocation)-1;
	inUseByte[0] = 'a';
}