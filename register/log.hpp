#pragma once
#include <string>

struct LogEntry
{
    int index;
    std::string key;
    std::string value;
    bool isValid;
};