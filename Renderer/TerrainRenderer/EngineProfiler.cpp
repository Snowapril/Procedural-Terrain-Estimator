#include "EngineProfiler.hpp"
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <string>


double EngineProfiler::secondsPerCount = 0.0;
std::unordered_map<std::string, Cell> EngineProfiler::profileInfo = std::unordered_map<std::string, Cell>();


EngineProfiler::EngineProfiler(const std::string& functionName)
{
	if (secondsPerCount == 0.0)
	{
		__int64 countsPerSecond;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
		secondsPerCount = 1.0 / countsPerSecond;
	}
	
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	startTime = currTime;
	currentLabel = functionName;
}

EngineProfiler::~EngineProfiler()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	double duration = (currTime - startTime) * secondsPerCount;
	
	if (profileInfo.find(currentLabel) == profileInfo.end())
	{
		profileInfo[currentLabel] = { 1, duration };
	}
	else
	{
		profileInfo[currentLabel].callCount++;
		profileInfo[currentLabel].totalTime += duration;
	}
}

void EngineProfiler::logging(const std::string& logFileName)
{
	std::ofstream logFile(logFileName);

	logFile << std::left << std::setw(40) << "Function Name" << std::setw(8) << "Trial" << std::setw(12) << "Total Time" << std::setw(15) << "Average Time" << std::endl;
	for (const auto& cell : profileInfo)
		logFile << std::left << std::setw(40) << cell.first << std::setw(8) << cell.second.callCount << std::setw(12) <<
		cell.second.totalTime << std::setw(15) << (cell.second.totalTime / cell.second.callCount) << std::endl;

	logFile.close();
}
