#include "FunctionProfiler.h"

Profiler::Profiler()
{
	ClearProfilerData();
}

void Profiler::StartProfilerTimer()
{
	profilerTimeStart = std::chrono::high_resolution_clock::now();
}

long long Profiler::EndProfilerTimer(const char* name)
{
	profilerTimeEnd = std::chrono::high_resolution_clock::now();

	std::chrono::microseconds functionTime = std::chrono::duration_cast<std::chrono::microseconds>(profilerTimeEnd - profilerTimeStart);

	functionTimes.push_back(functionTime);

	return functionTime.count();
}

long long Profiler::GetFunctionAvgTime(const char* name)
{
	std::chrono::microseconds avgTime = std::chrono::microseconds::zero();	

	for (const auto& val : functionTimes)
	{
		avgTime += val;
	}

	return long long(avgTime.count() / functionTimes.size());
}

void Profiler::ClearProfilerData()
{
	functionTimes.clear();
	printf("PROFILER DATA CLEARED\n");	
}
