#pragma once

#include "CalculatorTypes.h"
#include "ILogger.h"

#include <string>

namespace SimpleMath
{
	enum class SimpleMathError : int;
}

class Application
{
  private:
	Task task_;
	ILogger* logger_ = nullptr;

  public:
	Application(ILogger* logger = nullptr) : logger_(logger) {};
	int run(int argc, char** argv);

  private:
	bool processArguments(int argc, char** argv);
	void makeTask(int argc, char** argv);
	void makeCalculate();
	void printResult();
	void printLog(const std::string& level, const std::string& message);
	std::string argsToJsonStr(int argc, char** argv);
	CalculatorErrorType convertMathError(SimpleMath::SimpleMathError error);
};