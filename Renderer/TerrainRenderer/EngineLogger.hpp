/**
* @file		EngineLogger.hpp
* @author	Shinjihong
* @date		15 July 2018
* @version	1.0.0
* @brief	Logger static class for IndieEngine.
* @details	provide all logging features using spdlog open source library .
* @see		
*/

#ifndef ENGINE_LOGGER_HPP
#define ENGINE_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <memory>
#include <unordered_map>

class EngineLogger
{
public:
	static std::shared_ptr<spdlog::logger> getConsole(void);
};


#endif