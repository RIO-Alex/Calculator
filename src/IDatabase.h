#pragma once

#include <optional>
#include <unordered_map>
#include <string>
#include "CalculatorTypes.h"

class IDatabase
{
  public:
	virtual ~IDatabase() = default;
    
	virtual void warmUpCache() = 0;
    virtual std::optional<Task> getRecord(const Task& task) = 0;
    virtual void writeRecord(const Task& task) = 0;

  protected:
    std::unordered_map<std::string, Task> cache_;
};