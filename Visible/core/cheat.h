#pragma once

#include <chrono>
#include <Windows.h>
#include <filesystem>
#include "../dependencies/vmt/vmt.h"
#include "../dependencies/spdlog/spdlog.h"
#include "../dependencies/spdlog/fmt/ostr.h"
#include "../dependencies/spdlog/stopwatch.h"
#include "../dependencies/spdlog/sinks/basic_file_sink.h"
#include "../dependencies/spdlog/sinks/stdout_sinks.h"
#include "../dependencies/spdlog/sinks/rotating_file_sink.h"
#include "../dependencies/spdlog/sinks/wincolor_sink.h"

namespace cheat
{
	HINSTANCE dll_handle;
	void startup(HINSTANCE handle);

	namespace console
	{
		inline std::shared_ptr<spdlog::logger> logger;
		inline FILE* out{};

		inline void init_logger()
		{
			AllocConsole();
			SetConsoleTitleA(("cheat"));
			freopen_s(&out, ("CONOUT$"), ("w"), stdout);

			std::filesystem::create_directory("C:/logs/");
		
			std::vector<spdlog::sink_ptr> sinks;
			sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
			sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("C:\\logs\\log.txt", true));

			logger = std::make_shared<spdlog::logger>("main", begin(sinks), end(sinks));

			spdlog::set_default_logger(logger);
			spdlog::set_pattern("[%T] [%^%l%$] %v");
			spdlog::set_level(spdlog::level::trace);

			spdlog::info("Initialized Console");
		}
	}
}