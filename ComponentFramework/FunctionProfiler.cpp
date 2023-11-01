#include "FunctionProfiler.h"

Profiler::Profiler(bool useProfiler_)
{
	useProfiler = useProfiler_;
	ClearProfilerData();
}

void Profiler::StartProfilerTimer()
{
	if (useProfiler)
	{
		profilerTimeStart = std::chrono::high_resolution_clock::now();
	}
}

void Profiler::EndProfilerTimer(const char* name)
{
	if (useProfiler)
	{
		profilerTimeEnd = std::chrono::high_resolution_clock::now();

		std::chrono::microseconds functionTime = std::chrono::duration_cast<std::chrono::microseconds>(profilerTimeEnd - profilerTimeStart);

		functionTimes.push_back(functionTime);

		if (name != nullptr)
		{
			printf("%s TIME: %5lld\n", name, functionTime.count());
		}
		else
		{
			printf("FUNCTION TIME: %5lld\n", functionTime.count());
		}
	}
}

void Profiler::PrintFunctionAvgTime(const char* name)
{
	if (useProfiler)
	{
		std::chrono::microseconds avgTime = std::chrono::microseconds::zero();
		for (const auto& val : functionTimes)
		{
			avgTime += val;
		}
		if (name != nullptr)
		{
			printf("%s AVERAGE TIME: %5lld\n", name, (avgTime.count() / functionTimes.size()));
		}
		else
		{
			printf("FUNCTION AVERAGE TIME: %5lld\n", (avgTime.count() / functionTimes.size()));
		}
	}
}

void Profiler::ClearProfilerData()
{
	if (useProfiler)
	{
		functionTimes.clear();
		printf("PROFILER DATA CLEARED\n");
	}
}
