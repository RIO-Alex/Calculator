#pragma once

#include "CalculatorTypes.h"
#include "ILogger.h"
#include "IDatabase.h"

#include <memory>
#include <string>

namespace SimpleMath
{
	enum class SimpleMathError : int;
}

class Application
{
  private:
	Task task_;
	ILogger& logger_;
	std::unique_ptr<IDatabase> database_;

  public:
	Application(ILogger& logger, std::unique_ptr<IDatabase> database = nullptr);
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