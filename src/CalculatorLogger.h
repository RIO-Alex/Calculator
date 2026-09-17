#pragma once

#include "ILogger.h"

#include <memory>

class CalculatorLogger : public ILogger
{
  private:
	struct Impl;
	std::unique_ptr<Impl> pImpl;

  private:
	CalculatorLogger();

	CalculatorLogger(const CalculatorLogger&) = delete;
	CalculatorLogger& operator=(const CalculatorLogger&) = delete;

  public:
	static ILogger& getInstance();
	~CalculatorLogger() override;

	void info(const std::string& message) override;
	void warning(const std::string& message) override;
	void error(const std::string& message) override;
	void debug(const std::string& message) override;
};