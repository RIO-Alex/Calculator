#pragma once

#include "ILogger.h"

#include <string>

namespace SimpleMath
{
	enum class SimpleMathError : int;
}

class Application
{
  public:
	enum class CalculatorErrorType
	{
		EC_NONE,
		EC_NOTENOUGHARGUMENTS,
		EC_INVALIDNUMBER,
		EC_INVALIDOPERATION,
		EC_ARGUMENTOVERFLOW,
		EC_UNKNOWNERROR,

		EC_MATH_OVERFLOW,
		EC_MATH_DIVIDEBYZERO,
		EC_MATH_NEGATIVEEXPONENT,
		EC_MATH_NEGATIVEFACTORIALARGUMENT
	};

	struct Task
	{
		int first{0};
		int second{0};
		char operation{'?'};
		int result{0};
		CalculatorErrorType errorType{CalculatorErrorType::EC_NONE};

		void reset()
		{
			first = 0;
			second = 0;
			operation = '?';
			result = 0;
			errorType = CalculatorErrorType::EC_NONE;
		}
	};

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
	Application::CalculatorErrorType convertMathError(SimpleMath::SimpleMathError error);
};