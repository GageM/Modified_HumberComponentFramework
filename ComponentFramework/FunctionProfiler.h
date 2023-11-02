#pragma once
#include <chrono>
#include <vector>

class Profiler
{
public:
	Profiler();

	void StartProfilerTimer();

	long long EndProfilerTimer(const char* name = nullptr);

	void ClearProfilerData();

	long long GetFunctionAvgTime(const char* name = nullptr);
private:


	std::vector<std::chrono::microseconds> functionTimes;
	std::chrono::time_point<std::chrono::high_resolution_clock> profilerTimeStart;
	std::chrono::time_point<std::chrono::high_resolution_clock> profilerTimeEnd;
};