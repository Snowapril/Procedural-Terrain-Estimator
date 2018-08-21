#include "EngineLogger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>
#include <memory>
#include <filesystem>



std::shared_ptr<spdlog::logger> EngineLogger::getConsole(void) 
{
	namespace fs = std::experimental::filesystem;
	
	static std::shared_ptr<spdlog::logger> console = nullptr;
	
	if (console.get() == nullptr)
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

#ifdef _DEBUG
		
		constexpr const char* logDir = "./logs";
		constexpr const char* logPath = "./logs/setuplog.txt";

		if (!fs::exists(logDir)) {
			fs::create_directory(logDir);
		}

		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath));
		console = std::make_shared<spdlog::logger>("setup", sinks.begin(), sinks.end());
		console->set_level(spdlog::level::trace);
#else
		console = std::make_shared<spdlog::logger>("setup", sinks.begin(), sinks.end());
		console->set_level(spdlog::level::info);
#endif
	}

	return console; 
}