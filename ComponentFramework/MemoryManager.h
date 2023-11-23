#pragma once
#include <iostream>

#include <memory>

class MemoryMonitor
{
public:
	MemoryMonitor(MemoryMonitor& other) = delete;
	void operator =(const MemoryMonitor& other) = delete;

	static MemoryMonitor& GetInstance();

	void Print();
	void Print(std::size_t numBytes, bool isAllocating);

	inline size_t getMemoryUsage() const { return currentMemoryUsage; }
	inline size_t getMaxMemoryUsage() const { return maxMemoryUsage; }

	void setMemoryUsage(size_t a) {
		currentMemoryUsage = a;
		if (currentMemoryUsage > maxMemoryUsage)
			maxMemoryUsage = currentMemoryUsage;
	}

	~MemoryMonitor();

protected:
	MemoryMonitor();

	size_t currentMemoryUsage;
	size_t maxMemoryUsage;
	bool enablePrintText;
};

// Memory Manager Values
#define KB (1024)
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)

// THESE VALUES CONTROL THE SIZE OF EACH MEMORY SEGMENT
// AVAILABLE MEMORY IN EACH SEGMENT WILL BE 1 BYTE LESS Eg, 64 Bytes -> 63 Bytes
#define Size0 64
#define Size1 128
#define Size2 256
#define Size3 512
#define Size4 1 * KB
#define Size5 64 * KB
#define Size6 2 * MB

// THESE VALUES CONTROL THE AMOUNT OF MEMORY SEGMENTS OF EACH SIZE
#define Count0 55000	// ~3MB
#define Count1 8000		// ~1MB
#define Count2 2000		// 500KB
#define Count3 300		// ~1.5MB
#define Count4 100		// 100KB
#define Count5 100      // ~6MB 
#define Count6 20       // 40MB

						// ~52MB Total
class MemoryManager
{
public:
	MemoryManager(MemoryManager& other) = delete;
	void operator =(const MemoryManager& other) = delete;

	static MemoryManager& GetInstance();

	~MemoryManager();

	// Allocate memory from preallocated cache
	void* allocate(const size_t numBytes); 
	// Return memory to preallocated cache
	void deallocate(void* memoryLocation);

	// We have 5 large blocks of different sized memory segments
	char blockSet0[Size0 * Count0];
	char blockSet1[Size1 * Count1];
	char blockSet2[Size2 * Count2];
	char blockSet3[Size3 * Count3];
	char blockSet4[Size4 * Count4];
	char blockSet5[Size5 * Count5];
	char blockSet6[Size6 * Count6];

private:
	size_t segmentCount[7]{ Count0, Count1, Count2, Count3, Count4, Count5, Count6 };
	size_t segmentSize[7]{ Size0, Size1, Size2, Size3, Size4, Size5, Size6 };
	char* memoryBlocks[7]{ blockSet0, blockSet1, blockSet2, blockSet3, blockSet4, blockSet5, blockSet6 };
	int blockCount = 7;

	MemoryManager();
};

