#include "simplemath.h"

#include <getopt.h>

#include <cstdlib>
#include <iostream>

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
};

CalculatorErrorType convertMathError(SimpleMath::SimpleMathError error)
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

void printResult(const Task& task)
{
	if (task.errorType != CalculatorErrorType::EC_NONE)
	{
		switch (task.errorType)
		{
			case CalculatorErrorType::EC_NOTENOUGHARGUMENTS:
				printf("Error: Not enough arguments. Use --help or -h for usage information.\n");
				return;
			case CalculatorErrorType::EC_INVALIDNUMBER:
				printf("Error: Invalid number format. Use --help or -h for usage information.\n");
				return;
			case CalculatorErrorType::EC_INVALIDOPERATION:
				printf("Error: Invalid or unknown operation. Use --help or -h for usage information.\n");
				return;
			case CalculatorErrorType::EC_ARGUMENTOVERFLOW:
				printf("Error: Argument value is out of range. Use --help or -h for usage information.\n");
				return;
			case CalculatorErrorType::EC_MATH_OVERFLOW:
				printf("Math Error: Calculation result overflowed.\n");
				return;
			case CalculatorErrorType::EC_MATH_DIVIDEBYZERO:
				printf("Math Error: Division by zero is undefined.\n");
				return;
			case CalculatorErrorType::EC_MATH_NEGATIVEEXPONENT:
				printf("Math Error: Exponent cannot be negative.\n");
				return;
			case CalculatorErrorType::EC_MATH_NEGATIVEFACTORIALARGUMENT:
				printf("Math Error: Factorial is not defined for negative numbers.\n");
				return;
			default:
				printf("Error: Unknown error occurred.\n");
				return;
		}
	}

	printf("Result: %i\n", task.result);
}

int getNumber(char** argv, int index, CalculatorErrorType& error, bool* isFactorial)
{
	char* endptr = nullptr;
	errno = 0;
	long value = strtol(argv[index], &endptr, 10);

	if (endptr != nullptr && *endptr != '\0')
	{
		if (*endptr == '!' && *(endptr + 1) == '\0' && isFactorial != nullptr)
		{
			*isFactorial = true;
		}
		else
		{
			error = CalculatorErrorType::EC_INVALIDNUMBER;
			return -1;
		}
	}

	if (errno == ERANGE || value < INT_MIN || value > INT_MAX)
	{
		error = CalculatorErrorType::EC_ARGUMENTOVERFLOW;
		return -1;
	}

	return static_cast<int>(value);
}

bool processArguments(int argc, char** argv)
{
	struct option longOptions[] = {{"help", no_argument, nullptr, 'h'}, {nullptr, 0, nullptr, 0}};

	int opt = -1;
	while ((opt = getopt_long(argc, argv, "h", longOptions, nullptr)) != -1)
	{
		switch (opt)
		{
			case 'h':
				printf("Usage: %s <int_number1> <operation> <int_number2>\n", argv[0]);
				printf("Factorial usage: %s <int_number1>! or %s <int_number1> !\n", argv[0], argv[0]);
				printf("<int_number1> and <int_number2> should be valid integers within the range of int type.\n");
				printf("Result will be displayed as an integer.\n");
				printf("Available operations:\n"
					   "+ -- Add\n"
					   "- -- Subtract\n"
					   "x -- Multiply\n"
					   "/ -- Divide\n"
					   "^ -- Power\n"
					   "N! -- Factorial\n");
				return true;
			default:
				return false;
		}
	}

	return false;
}

bool makeTask(int argc, char** argv, Task& task)
{
	if (argc < 2)
	{
		task.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		return false;
	}

	// First value
	CalculatorErrorType error{CalculatorErrorType::EC_NONE};
	bool bIsFactorial = false;
	task.first = getNumber(argv, 1, error, &bIsFactorial);
	if (error != CalculatorErrorType::EC_NONE)
	{
		task.errorType = error;
		return false;
	}

	if (bIsFactorial)
	{
		task.operation = '!';
		return true;
	}

	if (argc < 3)
	{
		task.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		return false;
	}

	// Operation
	char operation = argv[2][0];
	if (argv[2][1] != '\0' || (operation != '+' && operation != '-' && operation != 'x' && operation != '/' &&
							   operation != '^' && operation != '!'))
	{
		task.errorType = CalculatorErrorType::EC_INVALIDOPERATION;
		return false;
	}

	task.operation = operation;
	if (task.operation == '!')
	{
		return true;
	}

	if (argc < 4)
	{
		task.errorType = CalculatorErrorType::EC_NOTENOUGHARGUMENTS;
		return false;
	}

	// Second value
	error = CalculatorErrorType::EC_NONE;
	task.second = getNumber(argv, 3, error, nullptr);
	if (error != CalculatorErrorType::EC_NONE)
	{
		task.errorType = error;
		return false;
	}

	return true;
}

void makeCalculate(Task& task)
{
	SimpleMath::SimpleMathError mathError{SimpleMath::SimpleMathError::EC_NONE};
	switch (task.operation)
	{
		case '+':
			task.result = SimpleMath::add(task.first, task.second, mathError);
			break;
		case '-':
			task.result = SimpleMath::subtract(task.first, task.second, mathError);
			break;
		case 'x':
			task.result = SimpleMath::multiply_v2(task.first, task.second, mathError);
			break;
		case '/':
			task.result = SimpleMath::divide(task.first, task.second, mathError);
			break;
		case '^':
			task.result = SimpleMath::pow_iterative(task.first, task.second, mathError);
			break;
		case '!':
			task.result = SimpleMath::factorial_v2(task.first, mathError);
			break;

		default:
			break;
	}

	task.errorType = convertMathError(mathError);
}

int applicationRun(int argc, char** argv)
{
	if (processArguments(argc, argv))
	{
		return 0;
	}

	Task task;
	if (!makeTask(argc, argv, task))
	{
		printResult(task);
		return 1;
	}

	makeCalculate(task);
	printResult(task);
	return 0;
}

int main(int argc, char** argv)
{
	return applicationRun(argc, argv);
}
