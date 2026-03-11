#pragma once
#include <string>

struct Message
{
    int nodeId;
    int logIndex;
    std::string key;
    std::string value;
    bool isValid;

    Message()
    {
        nodeId = -1;
        logIndex = -1;
        key = "";
        value = "";
        isValid = false;
    }

    Message(int nId, int lIndex, std::string k, std::string v, bool valid)
    {
        nodeId = nId;
        logIndex = lIndex;
        key = k;
        value = v;
        isValid = valid;
    }
};