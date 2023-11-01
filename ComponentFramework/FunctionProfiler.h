#pragma once
#include <chrono>
#include <vector>

class Profiler
{
public:
	Profiler(bool useProfiler_);

	void StartProfilerTimer();

	void EndProfilerTimer(const char* name = nullptr);

	void ClearProfilerData();

	void PrintFunctionAvgTime(const char* name = nullptr);
private:
	bool useProfiler;

	std::vector<std::chrono::microseconds> functionTimes;
	std::chrono::time_point<std::chrono::high_resolution_clock> profilerTimeStart;
	std::chrono::time_point<std::chrono::high_resolution_clock> profilerTimeEnd;
};