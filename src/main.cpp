#include "Application.h"
#include "CalculatorLogger.h"

int main(int argc, char** argv)
{
	Application app(&CalculatorLogger::getInstance());
	return app.run(argc, argv);
}
