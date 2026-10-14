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

class MockDatabase : public IDatabase
{
  public:
    MOCK_METHOD(void, warmUpCache, (), (override));
    MOCK_METHOD(std::optional<Task>, getRecord, (const Task& task), (override));
    MOCK_METHOD(void, writeRecord, (const Task& task), (override));
};

/*
    SUCCESSFULL TESTS
*/

TEST(CalculatorTests_Success, SuccessfullOperation)
{
    MockLogger logger;
    EXPECT_CALL(logger, info("1 + 2 = 3")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 1, \"second\": 2, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests_Success, SuccessfullOperationWithFile)
{
    MockLogger logger;
    EXPECT_CALL(logger, info("10 / 2 = 5")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "./tests/test1.json";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests_Success, SuccessfullFactorialOperation)
{
    MockLogger logger;
    EXPECT_CALL(logger, info("5! = 120")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 5, \"operation\": \"!\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests_Success, ProcessesHelpFlagShort)
{
    MockLogger logger;
    EXPECT_CALL(logger, info(testing::_)).Times(5);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "-h";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests_Success, ProcessesHelpFlagLong)
{
    MockLogger logger;
    EXPECT_CALL(logger, info(testing::_)).Times(5);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "--help";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_EQ(result, 0);
}

/*
    CALCULATOR ERRORS
*/

TEST(CalculatorTests_Errors, ReturnsErrorWhenNoArguments)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Error: Not enough arguments.")).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char* argv[] = { arg0 };

    int result = app.run(1, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_Errors, ReturnsErrorWhenMissingKey)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("JSON Out of Range Error:"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"somekey\": \"some value\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_Errors, ReturnsErrorWhenInvalidString)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("JSON Parse Error:"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "jh gds fhs adg faskf";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_Errors, ReturnsErrorWhenInvalidType)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("JSON Type Error:"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 1, \"second\": \"2\", \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_Errors, ReturnsErrorOnFirstArgumentOverflow)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("First argument is out of range"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    // 3000000000 > INT_MAX (2147483647)
    char arg1[] = "{\"first\": 300000000000, \"second\": 1, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_Errors, ReturnsErrorOnSecondArgumentOverflow)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("First argument is out of range"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    // 3000000000 > INT_MAX (2147483647)
    char arg1[] = "{\"first\": 300000000000, \"second\": 1, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_Errors, ReturnsErrorOnInvalidOperationSymbol)
{
    MockLogger logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("Invalid or unknown operation"))).Times(1);
    EXPECT_CALL(logger, warning("Use --help or -h for usage information.")).Times(1);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 10, \"second\": 2, \"operation\": \"%\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

/*
    MATH ERRORS
*/

TEST(CalculatorTests_MathErrors, ReturnsMathErrorOnMathOverflow)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Calculation result overflowed.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 1000000, \"second\": 1000000, \"operation\": \"x\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_MathErrors, ReturnsMathErrorOnDivideByZero)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Division by zero is undefined.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 10, \"second\": 0, \"operation\": \"/\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_MathErrors, ReturnsMathErrorOnNegativeExponent)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Exponent cannot be negative.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 2, \"second\": -3, \"operation\": \"^\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

TEST(CalculatorTests_MathErrors, ReturnsMathErrorOnNegativeFactorialArgument)
{
    MockLogger logger;
    EXPECT_CALL(logger, error("Math Error: Factorial is not defined for negative numbers.")).Times(1);
    EXPECT_CALL(logger, warning(testing::_)).Times(0);

    Application app(logger);
    char arg0[] = "calculator";
    char arg1[] = "{\"first\": -5, \"second\": 0, \"operation\": \"!\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);

    EXPECT_NE(result, 0);
}

/*
    DATABASE
*/

TEST(CalculatorTests_Database, CallsWarmUpCacheOnInitialization)
{
    MockLogger logger;
    auto db = std::make_unique<MockDatabase>();

    EXPECT_CALL(*db, warmUpCache()).Times(1);

    Application app(logger, std::move(db));
}

TEST(CalculatorTests_Database, ReturnsCachedRecord)
{
    MockLogger logger;
    auto db = std::make_unique<MockDatabase>();

    EXPECT_CALL(*db, warmUpCache()).Times(1);

    Task cachedTask{ 5, 5, '+', 10, CalculatorErrorType::EC_NONE };
    EXPECT_CALL(*db, getRecord(testing::_)).WillOnce(testing::Return(cachedTask));
    EXPECT_CALL(*db, writeRecord(testing::_)).Times(0);
    EXPECT_CALL(logger, info("5 + 5 = 10")).Times(1);

    Application app(logger, std::move(db));

    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 5, \"second\": 5, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);
    EXPECT_EQ(result, 0);
}

TEST(CalculatorTests_Database, CalculateAndWriteRecordToDatabase)
{
    MockLogger logger;
    auto db = std::make_unique<MockDatabase>();

    EXPECT_CALL(*db, warmUpCache()).Times(1);
    EXPECT_CALL(*db, getRecord(testing::_)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(*db, writeRecord(testing::_)).Times(1);
    EXPECT_CALL(logger, info("2 + 3 = 5")).Times(1);

    Application app(logger, std::move(db));

    char arg0[] = "calculator";
    char arg1[] = "{\"first\": 2, \"second\": 3, \"operation\": \"+\"}";
    char* argv[] = { arg0, arg1 };

    int result = app.run(2, argv);
    EXPECT_EQ(result, 0);
}