#include "PqDatabase.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>

namespace {
class QuietLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void warning(const std::string&) override {}
    void error(const std::string&) override {}
    void debug(const std::string&) override {}
};

struct ResultDeleter {
    void operator()(PGresult* result) const { PQclear(result); }
};
struct ConnectionDeleter {
    void operator()(PGconn* connection) const { PQfinish(connection); }
};

// Borrowed C API arguments never own resources. Results and connections do.
class PqTests : public testing::Test {
protected:
    QuietLogger logger;
    std::unique_ptr<PGconn, ConnectionDeleter> admin{PQconnectdb("")};
    void sql(const char* query) {
        std::unique_ptr<PGresult, ResultDeleter> result{PQexec(admin.get(), query)};
        ASSERT_TRUE(result);
        ASSERT_EQ(PQresultStatus(result.get()), PGRES_COMMAND_OK)
            << PQresultErrorMessage(result.get());
    }
    int count() {
        std::unique_ptr<PGresult, ResultDeleter> result{
            PQexec(admin.get(), "SELECT count(*) FROM calculations")};
        if (!result || PQresultStatus(result.get()) != PGRES_TUPLES_OK)
            throw std::runtime_error("Cannot count calculations");
        return std::stoi(PQgetvalue(result.get(), 0, 0));
    }
    void SetUp() override {
        ASSERT_TRUE(admin);
        ASSERT_EQ(PQstatus(admin.get()), CONNECTION_OK) << PQerrorMessage(admin.get());
        PqDatabase database(logger, {});
        sql("TRUNCATE calculations");
    }
};

TEST_F(PqTests, PersistsWarmsAndNormalizesOperations) {
    {
        PqDatabase database(logger, {});
        database.warmUpCache();
        EXPECT_FALSE(database.getRecord(Task{2, 3, '+'}));
        database.writeRecord(Task{2, 3, '+', 5});
        database.writeRecord(Task{2, 3, 'x', 6});
        database.writeRecord(Task{8, 2, '/', 4});
        database.writeRecord(Task{5, 0, '!', 120});
        database.writeRecord(Task{10, 0, '/', -1, CalculatorErrorType::EC_MATH_DIVIDEBYZERO});
    }
    PqDatabase restarted(logger, {});
    restarted.warmUpCache();
    auto sum = restarted.getRecord(Task{3, 2, '+'});
    ASSERT_TRUE(sum);
    EXPECT_EQ(sum->result, 5);
    EXPECT_EQ(sum->errorType, CalculatorErrorType::EC_NONE);
    auto product = restarted.getRecord(Task{3, 2, 'x'});
    ASSERT_TRUE(product);
    EXPECT_EQ(product->result, 6);
    EXPECT_FALSE(restarted.getRecord(Task{2, 8, '/'}));
    auto factorial = restarted.getRecord(Task{5, 99, '!'});
    ASSERT_TRUE(factorial);
    EXPECT_EQ(factorial->result, 120);
    auto error = restarted.getRecord(Task{10, 0, '/'});
    ASSERT_TRUE(error);
    EXPECT_EQ(error->errorType, CalculatorErrorType::EC_MATH_DIVIDEBYZERO);
    EXPECT_EQ(count(), 5);
}

TEST_F(PqTests, KeepsCacheWhenInsertFails) {
    PqDatabase database(logger, {});
    sql("ALTER TABLE calculations ADD CONSTRAINT reject_insert CHECK (false)");
    EXPECT_THROW(database.writeRecord(Task{7, 9, '+', 16}), std::runtime_error);
    auto cached = database.getRecord(Task{9, 7, '+'});
    ASSERT_TRUE(cached);
    EXPECT_EQ(cached->result, 16);
    EXPECT_EQ(count(), 0);
    sql("ALTER TABLE calculations DROP CONSTRAINT reject_insert");
}

TEST_F(PqTests, ReleasesResourcesOnQueryErrors) {
    PqDatabase database(logger, {});
    sql("ALTER TABLE calculations RENAME TO hidden_calculations");
    EXPECT_THROW(database.warmUpCache(), std::runtime_error);
    EXPECT_THROW(database.writeRecord(Task{1, 2, '+', 3}), std::runtime_error);
    sql("ALTER TABLE hidden_calculations RENAME TO calculations");
    sql("ALTER TABLE calculations RENAME COLUMN result TO hidden_result");
    EXPECT_THROW(database.warmUpCache(), std::runtime_error);
    sql("ALTER TABLE calculations RENAME COLUMN hidden_result TO result");
}

TEST_F(PqTests, ReleasesResourcesOnConstructorError) {
    sql("ALTER ROLE calculator_test SET default_transaction_read_only TO on");
    EXPECT_THROW(PqDatabase(logger, {}), std::runtime_error);
    sql("ALTER ROLE calculator_test RESET default_transaction_read_only");
}

TEST_F(PqTests, ReleasesFailedConnection) {
    PqDatabase::DbConfig config;
    config.dbName = "database_that_does_not_exist";
    EXPECT_THROW(PqDatabase(logger, config), std::runtime_error);
}
}
