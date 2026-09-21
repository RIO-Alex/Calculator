#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ILogger.h"
#include "Application.h"

class MockLogger : public ILogger
{
  public:
	MOCK_METHOD(void, info, (const std::string& message), (override));
    MOCK_METHOD(void, warning, (const std::string& message), (override));
    MOCK_METHOD(void, error, (const std::string& message), (override));
    MOCK_METHOD(void, debug, (const std::string& message), (override));
};

/*
    SUCCESSFULL TESTS
*/

TEST(CalculatorTests, SuccessfullOperation)
{
    MockLogger logger;
    EXPECT_CALL(logger, info("1 + 2 = 3")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 1, \"second\": 2, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests, SuccessfullOperationWithFile)
{
    MockLogger logger;
    EXPECT_CALL(logger, info("10 / 2 = 5")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "./tests/test1.json";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests, SuccessfullFactorialOperation)
{
    MockLogger logger;
    EXPECT_CALL(logger, info("5! = 120")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 5, \"operation\": \"!\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests, ProcessesHelpFlagShort)
{
    MockLogger logger;
    EXPECT_CALL(logger, info(testing::_)).Times(5);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "-h";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests, ProcessesHelpFlagLong)
{
    MockLogger logger;
    EXPECT_CALL(logger, info(testing::_)).Times(5);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "--help";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests, HandleNullLogger)
{
    Application app(nullptr);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 2, \"second\": 3, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

/*
    CALCULATOR ERRORS
*/

TEST(CalculatorTests, ReturnsErrorWhenNoArguments)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Error: Not enough arguments.")).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char* argv[] = { arg0 };

    int result = app.run(1, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsErrorWhenMissingKey)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("JSON Out of Range Error:"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"somekey\": \"some value\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsErrorWhenInvalidString)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("JSON Parse Error:"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "jh gds fhs adg faskf";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsErrorWhenInvalidType)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("JSON Type Error:"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 1, \"second\": \"2\", \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsErrorOnFirstArgumentOverflow)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("First argument is out of range"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    // 3000000000 > INT_MAX (2147483647)
    char arg1[] = "{\"first\": 300000000000, \"second\": 1, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsErrorOnSecondArgumentOverflow)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("First argument is out of range"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    // 3000000000 > INT_MAX (2147483647)
    char arg1[] = "{\"first\": 300000000000, \"second\": 1, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsErrorOnInvalidOperationSymbol)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("Invalid or unknown operation"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 10, \"second\": 2, \"operation\": \"%\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

/*
    MATH ERRORS
*/

TEST(CalculatorTests, ReturnsMathErrorOnMathOverflow)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Calculation result overflowed.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 1000000, \"second\": 1000000, \"operation\": \"x\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsMathErrorOnDivideByZero)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Division by zero is undefined.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 10, \"second\": 0, \"operation\": \"/\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsMathErrorOnNegativeExponent)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Exponent cannot be negative.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 2, \"second\": -3, \"operation\": \"^\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests, ReturnsMathErrorOnNegativeFactorialArgument)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Factorial is not defined for negative numbers.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(&logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": -5, \"second\": 0, \"operation\": \"!\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}