#pragma once
#include <iostream>

#include <memory>
#define KB (1024)
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)


// THESE VALUES CONTROL THE SIZE OF EACH MEMORY SEGMENT
// AVAILABLE MEMORY IN EACH SEGMENT WILL BE 1 BYTE LESS Eg, 64 Bytes -> 63 Bytes
#define Size0 64
#define Size1 128
#define Size2 256
#define Size3 (10 * KB)
#define Size4 (1 * MB)

// THESE VALUES CONTROL THE AMOUNT OF MEMORY SEGMENTS OF EACH SIZE
#define Count0 55000
#define Count1 500
#define Count2 50
#define Count3 50
#define Count4 50


class MemoryMonitor
{
public:
	MemoryMonitor(MemoryMonitor& other) = delete;
	void operator =(const MemoryMonitor& other) = delete;

	static MemoryMonitor& GetInstance();

	void Print();
	void Print(std::size_t numBytes, bool isAllocating);

	size_t getMemoryUsage() { return currentMemoryUsage; }
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

private:
	size_t segmentCount[5]{ Count0, Count1, Count2, Count3, Count4 };
	size_t segmentSize[5]{ Size0, Size1, Size2, Size3, Size4 };

	MemoryManager();
};

