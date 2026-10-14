#pragma once

#include "IDatabase.h"
#include "ILogger.h"
#include "postgresql/libpq-fe.h"
#include <memory>

class PqDatabase : public IDatabase
{
  private:
    struct PqDeleter
    {
        void operator()(PGconn* connection) const noexcept
        {
            if (connection)
                PQfinish(connection);
        }
    };

    struct PqResultDeleter
    {
        void operator()(PGresult* result)
        {
            if (result)
                PQclear(result);
        }
    };

	std::unique_ptr<PGconn, PqDeleter> connection_;
    ILogger& logger_;

  public:
    struct DbConfig
    {
        std::string host = "localhost";
        unsigned int port = 5432;
        std::string username = "rio";
        std::string password = "riopassword10";
        std::string dbName = "calculator_db";
    };

	PqDatabase(ILogger& logger, const DbConfig& connectionConfig);
	~PqDatabase() override;

	virtual void warmUpCache() override;
    virtual std::optional<Task> getRecord(const Task& task) override;
    virtual void writeRecord(const Task& task) override;

  private:
	PqDatabase(const PqDatabase&) = delete;
	PqDatabase& operator=(const PqDatabase&) = delete;

    std::unique_ptr<PGconn, PqDeleter> connect(const DbConfig& config);
    std::string getOperationKey(const Task& task);
    static void processNotice(void* arg, const char* message);
};