#include "Application.h"

#include "simplemath.h"

#include <getopt.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

int Application::run(int argc, char** argv)
{
	if (!logger_)
		printf("[WARN] logger was not found! Using standard output...\n");

	if (processArguments(argc, argv))
		return 0;

	try
	{
		makeTask(argc, argv);
		makeCalculate();
	}
	catch (const nlohmann::json::parse_error& e)
	{
		task_.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		printLog("error", std::string("JSON Parse Error: ") + e.what());
	}
	catch (const nlohmann::json::type_error& e)
	{
		task_.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		printLog("error", std::string("JSON Type Error: ") + e.what());
	}
	catch (const nlohmann::json::out_of_range& e)
	{
		task_.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		printLog("error", std::string("JSON Out of Range Error: ") + e.what());
	}
	catch (const std::exception& e)
	{
		printLog("error", std::string("Error: ") + e.what());

		if (task_.errorType == Application::CalculatorErrorType::EC_NONE) // if error unknowed
			task_.errorType = Application::CalculatorErrorType::EC_UNKNOWNERROR;
	}
	catch (...)
	{
		printLog("error", "Unknown exception occurred.");
		if (task_.errorType == Application::CalculatorErrorType::EC_NONE) // if error unknowed
			task_.errorType = Application::CalculatorErrorType::EC_UNKNOWNERROR;
	}

	// 0 if all is ok, else - 1
	int result = 0;
	if (task_.errorType != Application::CalculatorErrorType::EC_NONE)
		result = 1;

	printResult();
	return result;
}

bool Application::processArguments(int argc, char** argv)
{
	struct option longOptions[] = {{"help", no_argument, nullptr, 'h'}, {nullptr, 0, nullptr, 0}};

	optind = 1;
	opterr = 0;
	int opt = -1;
	while ((opt = getopt_long(argc, argv, "h", longOptions, nullptr)) != -1)
	{
		switch (opt)
		{
			case 'h':
				printLog("info", "Usage: calculator <json string>/<json file path>");
				printLog("info", "JSON structure: { \"first\": 1, \"operation\": \"+\", \"second\": 1 }");
				printLog("info", "first and second should be valid integers within the range of int type.");
				printLog("info", "Result will be displayed as an integer.");
				printLog("info", "Available operations:\n"
								 "+ -- Add\n"
								 "- -- Subtract\n"
								 "x -- Multiply\n"
								 "/ -- Divide\n"
								 "^ -- Power\n"
								 "! -- Factorial");
				return true;
			default:
				return false;
		}
	}

	return false;
}

void Application::makeTask(int argc, char** argv)
{
	if (argc < 2)
	{
		task_.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		throw std::invalid_argument("Not enough arguments.");
	}

	task_.reset();
	nlohmann::json jsonArgs = nlohmann::json::parse(argsToJsonStr(argc, argv), nullptr, true);

	auto firstValue = jsonArgs.at("first").get<int64_t>();
	if (firstValue > INT_MAX || firstValue < INT_MIN)
	{
		task_.errorType = CalculatorErrorType::EC_ARGUMENTOVERFLOW;
		throw std::out_of_range("First argument is out of range for int type.");
	}
	task_.first = static_cast<int>(firstValue);

	std::string inStr = jsonArgs.at("operation").get<std::string>();
	if (inStr.empty() || (inStr[0] != '+' && inStr[0] != '-' && inStr[0] != 'x' && inStr[0] != '/' && inStr[0] != '^' &&
						  inStr[0] != '!'))
	{
		task_.errorType = CalculatorErrorType::EC_INVALIDOPERATION;
		throw std::invalid_argument("Invalid or unknown operation.");
	}
	task_.operation = inStr[0];

	if (task_.operation != '!')
	{
		auto secondValue = jsonArgs.at("second").get<int64_t>();
		if (secondValue > INT_MAX || secondValue < INT_MIN)
		{
			task_.errorType = CalculatorErrorType::EC_ARGUMENTOVERFLOW;
			throw std::out_of_range("Second argument is out of range for int type.");
		}
		task_.second = static_cast<int>(secondValue);
	}
}

void Application::makeCalculate()
{
	SimpleMath::SimpleMathError mathError{SimpleMath::SimpleMathError::EC_NONE};
	switch (task_.operation)
	{
		case '+':
			task_.result = SimpleMath::add(task_.first, task_.second, mathError);
			break;
		case '-':
			task_.result = SimpleMath::subtract(task_.first, task_.second, mathError);
			break;
		case 'x':
			task_.result = SimpleMath::multiply_v2(task_.first, task_.second, mathError);
			break;
		case '/':
			task_.result = SimpleMath::divide(task_.first, task_.second, mathError);
			break;
		case '^':
			task_.result = SimpleMath::pow_iterative(task_.first, task_.second, mathError);
			break;
		case '!':
			task_.result = SimpleMath::factorial_v2(task_.first, mathError);
			break;

		default:
			break;
	}

	task_.errorType = convertMathError(mathError);
}

void Application::printResult()
{
	if (task_.errorType != CalculatorErrorType::EC_NONE)
	{
		switch (task_.errorType)
		{
			case CalculatorErrorType::EC_MATH_OVERFLOW:
				printLog("error", "Math Error: Calculation result overflowed.");
				return;
			case CalculatorErrorType::EC_MATH_DIVIDEBYZERO:
				printLog("error", "Math Error: Division by zero is undefined.");
				return;
			case CalculatorErrorType::EC_MATH_NEGATIVEEXPONENT:
				printLog("error", "Math Error: Exponent cannot be negative.");
				return;
			case CalculatorErrorType::EC_MATH_NEGATIVEFACTORIALARGUMENT:
				printLog("error", "Math Error: Factorial is not defined for negative numbers.");
				return;
			default:
				printLog("warn", "Use --help or -h for usage information.");
				return;
		}
	}

	// also we can use fmt lib or std::format() is we use C++20
	std::ostringstream os;
	if (task_.operation == '!')
		os << task_.first << task_.operation;
	else
		os << task_.first << ' ' << task_.operation << ' ' << task_.second;

	os << " = " << task_.result;

	printLog("info", os.str());
}

void Application::printLog(const std::string& level, const std::string& message)
{
	if (logger_)
	{
		if (level == "warn")
			logger_->warning(message);
		else if (level == "error")
			logger_->error(message);
		else if (level == "debug")
			logger_->debug(message);
		else
			logger_->info(message);
	}
	else
		printf("%s\n", message.c_str());
}

std::string Application::argsToJsonStr(int argc, char** argv)
{
	if (argc < 2)
		return std::string();

	std::string result = "";
	for (int i = 1; i < argc; ++i)
	{
		result += argv[i];
		if (i < argc - 1)
			result += " ";
	}

	// check if it's file path
	try
	{
		if (fs::exists(result) && fs::is_regular_file(result))
		{
			std::ifstream file(result);
			if (file.is_open())
			{
				std::stringstream ss;
				ss << file.rdbuf();
				result = ss.str();
			}
		}
	}
	catch (...)
	{
		// Ignore exceptions here because I just want to check if input is file path. If not - it may be json str.
	}

	return result;
}

Application::CalculatorErrorType Application::convertMathError(SimpleMath::SimpleMathError error)
{
	switch (error)
	{
		case SimpleMath::SimpleMathError::EC_NONE:
			return CalculatorErrorType::EC_NONE;
		case SimpleMath::SimpleMathError::EC_OVERFLOW:
			return CalculatorErrorType::EC_MATH_OVERFLOW;
		case SimpleMath::SimpleMathError::EC_DIVIDEBYZERO:
			return CalculatorErrorType::EC_MATH_DIVIDEBYZERO;
		case SimpleMath::SimpleMathError::EC_NEGATIVEEXPONENT:
			return CalculatorErrorType::EC_MATH_NEGATIVEEXPONENT;
		case SimpleMath::SimpleMathError::EC_NEGATIVEFACTORIALARGUMENT:
			return CalculatorErrorType::EC_MATH_NEGATIVEFACTORIALARGUMENT;

		default:
			return CalculatorErrorType::EC_UNKNOWNERROR;
	}
}