#include "PqDatabase.h"
#include <sstream>

PqDatabase::PqDatabase(ILogger& logger, const DbConfig& connectionConfig) 
                      : logger_(logger), connection_(connect(connectionConfig)) 
{
    PQsetNoticeProcessor(connection_.get(), &PqDatabase::processNotice, nullptr);

    const char* query = "CREATE TABLE IF NOT EXISTS calculations ("
                        "id BIGSERIAL PRIMARY KEY, "
                        "first_value INTEGER NOT NULL,"
                        "operation CHAR(1) NOT NULL, "
                        "second_value INTEGER NOT NULL, "
                        "result INTEGER NOT NULL, "
                        "status INTEGER NOT NULL "
                        ");";

    std::unique_ptr<PGresult, PqResultDeleter> result(PQexec(connection_.get(), query));

    if (!result)
        throw std::runtime_error("Failed to execute CREATE TABLE query");

    if (PQresultStatus(result.get()) != PGRES_COMMAND_OK)
        throw std::runtime_error(std::string("Failed to create calculations table: ") 
                                + PQresultErrorMessage(result.get()));
}

PqDatabase::~PqDatabase() = default;

void PqDatabase::warmUpCache()
{
    const char* query = "SELECT first_value,operation,second_value,result,status FROM calculations";
    std::unique_ptr<PGresult, PqResultDeleter> result(PQexec(connection_.get(), query));

    if (!result)
        throw std::runtime_error("Failed to execute SELECT query");

    if (PQresultStatus(result.get()) != PGRES_TUPLES_OK)
        throw std::runtime_error(std::string("Failed to select values from calculations table: ") 
                                + PQresultErrorMessage(result.get()));

    std::unordered_map<std::string, Task> localCache;
    const int rows = PQntuples(result.get());
    for (int i = 0; i < rows; i++)
    {
        int first, second, operationResult, errorCode;
        char operation;
        try 
        {
            first = std::stoi(PQgetvalue(result.get(), i, 0));
            operation = PQgetvalue(result.get(), i, 1)[0];
            second = std::stoi(PQgetvalue(result.get(), i, 2));
            operationResult = std::stoi(PQgetvalue(result.get(), i, 3));
            errorCode = std::stoi(PQgetvalue(result.get(), i, 4));

            Task task{ first, second, operation, operationResult, static_cast<CalculatorErrorType>(errorCode) };
            localCache[getOperationKey(task)] = task;
        }
        catch (const std::invalid_argument& e)
        {
            logger_.error("[PqDatabase] Invalid value in row " + std::to_string(i) + ": " + e.what());
            continue;
        }
        catch (const std::out_of_range& e)
        {
            logger_.error("[PqDatabase] Value out of range in row " + std::to_string(i) + ": " + e.what());
            continue;
        }
    }

    cache_ = std::move(localCache);
}

std::optional<Task> PqDatabase::getRecord(const Task& task)
{
    std::string key = getOperationKey(task);
    auto it = cache_.find(key);

    if (it == cache_.end())
        return std::nullopt;

    return it->second;
}

void PqDatabase::writeRecord(const Task& task)
{
    // Keep expensive results even if persistent storage fails.
    cache_[getOperationKey(task)] = task;
    const char* query = "INSERT INTO calculations "
                        "(first_value, operation, second_value, result, status) "
                        "VALUES ($1, $2, $3, $4, $5);";

    std::string first = std::to_string(task.first);
    std::string operation = std::string(1, task.operation);
    std::string second = std::to_string(task.second);
    std::string operationResult = std::to_string(task.result);
    std::string errorCode = std::to_string(static_cast<int>(task.errorType));

    const char* values[] = {
        first.c_str(),
        operation.c_str(),
        second.c_str(),
        operationResult.c_str(),
        errorCode.c_str()
    };

    std::unique_ptr<PGresult, PqResultDeleter> result(
        PQexecParams(connection_.get(), query, 5, nullptr, values, nullptr, nullptr, 0)
    );

    if (!result)
        throw std::runtime_error("Failed to execute INSERT INTO query");

    if (PQresultStatus(result.get()) != PGRES_COMMAND_OK)
        throw std::runtime_error(std::string("Failed to insert values to calculations table: ") 
                                + PQresultErrorMessage(result.get()));

}

std::unique_ptr<PGconn, PqDatabase::PqDeleter> PqDatabase::connect(const DbConfig& config)
{
    const char* keys[] = {
        "host",
        "port",
        "dbname",
        "user",
        "password",
        nullptr
    };

    const std::string port = config.port == 0 ? "" : std::to_string(config.port);
    const char* values[] = {
        config.host.c_str(),
        port.c_str(),
        config.dbName.c_str(),
        config.username.c_str(),
        config.password.c_str(),
        nullptr
    };

    std::unique_ptr<PGconn, PqDeleter> connection(PQconnectdbParams(keys, values, 0));

    if (!connection)
        throw std::runtime_error("Failed to create PostgreSQL connection");

    if (PQstatus(connection.get()) != CONNECTION_OK)
        throw std::runtime_error(std::string("PostgreSQL connection failed: ") + PQerrorMessage(connection.get()));

    return connection;
}

std::string PqDatabase::getOperationKey(const Task& task)
{
    std::stringstream ss;

	if ((task.operation == '+' || task.operation == 'x') && task.second < task.first)
        ss << task.second << task.operation << task.first;
    else if (task.operation == '!')
        ss << task.first << task.operation;
    else
        ss << task.first << task.operation << task.second;

    return ss.str();
}

void PqDatabase::processNotice(void* arg, const char* message)
{
    // here we can use logger for NOTICE messages.
    // I added it here, because I want to hide message about "table already exists"
}
