#pragma once

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