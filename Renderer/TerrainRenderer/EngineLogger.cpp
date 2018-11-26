#include "EngineLogger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>
#include <filesystem>
#include "obfuscator.hpp"


std::shared_ptr<spdlog::logger> EngineLogger::getConsole(void) 
{
	/// I prefer using static class than using Singleton pattern

	namespace fs = std::experimental::filesystem;
	
	static std::shared_ptr<spdlog::logger> console = nullptr;
	
	if (console.get() == nullptr)
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

#ifdef _DEBUG
		
		constexpr const char* logDir = OBFUSCATE("./logs");
		constexpr const char* logPath = OBFUSCATE("./logs/setuplog.txt");

		if (!fs::exists(logDir))
		{
			fs::create_directory(logDir);
		}

		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath));
		console = std::make_shared<spdlog::logger>(OBFUSCATE("setup"), sinks.begin(), sinks.end());
		/// at debug mode, print all level of log.
		console->set_level(spdlog::level::trace);
#else
		console = std::make_shared<spdlog::logger>(OBFUSCATE("setup"), sinks.begin(), sinks.end());
		/// if not at debug mode, print log which level is equal or larger than info level.
		console->set_level(spdlog::level::warn);
#endif
	}

	return console; 
}