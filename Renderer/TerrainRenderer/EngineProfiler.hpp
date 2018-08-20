/**
* @file		EngineProfiler.hpp
* @author	Shinjihong
* @date		19 July 2018
* @version	1.0.0
* @brief	Profiler class for profiling IndieEngine.
* @details	store all methods' call count and duration. at the last of this program, log all information into txt file.
* @see		
*/

#ifndef ENGINE_PROFILER_HPP
#define ENGINE_PROFILER_HPP

#include <unordered_map>

typedef struct _cellInfo {
	int callCount;
	double totalTime;
} Cell;

class EngineProfiler
{
private:
	static std::unordered_map<std::string, Cell> profileInfo;
	static double secondsPerCount;

	std::string currentLabel;
	__int64 startTime;
public:
	EngineProfiler(const std::string& functionName);
	~EngineProfiler();

	static void logging(const std::string& logFileName);
};

#ifdef _DEBUG
#define Profile() EngineProfiler(__FUNCTION__);
#else
#define Profile()
#endif

#endif