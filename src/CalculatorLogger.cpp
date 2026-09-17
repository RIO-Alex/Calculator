#include "CalculatorLogger.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

struct CalculatorLogger::Impl
{
	std::shared_ptr<spdlog::logger> logger;
};

CalculatorLogger::CalculatorLogger() : pImpl(std::make_unique<Impl>())
{
	pImpl->logger = spdlog::get("CalculatorLogger");
	if (!pImpl->logger)
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); // color console
		auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/calculator.log", 1024 * 1024 * 5,
																				5 // up to 5 files (5MB)
		);

		std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
		pImpl->logger = std::make_shared<spdlog::logger>("CalculatorLogger", sinks.begin(), sinks.end());

		spdlog::register_logger(pImpl->logger);
	}

	pImpl->logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$]: %v");
	pImpl->logger->set_level(spdlog::level::info);
}

CalculatorLogger::~CalculatorLogger() = default;

ILogger& CalculatorLogger::getInstance()
{
	static CalculatorLogger instance;
	return instance;
}

void CalculatorLogger::info(const std::string& message)
{
	pImpl->logger->info(message);
}

void CalculatorLogger::warning(const std::string& message)
{
	pImpl->logger->warn(message);
}

void CalculatorLogger::error(const std::string& message)
{
	pImpl->logger->error(message);
}

void CalculatorLogger::debug(const std::string& message)
{
	pImpl->logger->debug(message);
}
