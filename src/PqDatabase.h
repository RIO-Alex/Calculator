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
        // Empty values use libpq defaults and PG* environment variables.
        std::string host;
        unsigned int port = 0;
        std::string username;
        std::string password;
        std::string dbName;
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