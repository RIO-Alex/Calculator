#include "Application.h"
#include "CalculatorLogger.h"
#include "PqDatabase.h"

int main(int argc, char** argv)
{
	auto& logger = CalculatorLogger::getInstance();
	PqDatabase::DbConfig config;
	config.dbName = "calculator_db";
	config.username = "rio";
	std::unique_ptr<IDatabase> database;

	try
	{
		database = std::make_unique<PqDatabase>(logger, config);
	}
	catch (const std::exception& e)
	{
		logger.error(std::string("Failed to initialize database: ") + e.what());
	}

	Application app(logger, std::move(database));

	return app.run(argc, argv);
}
