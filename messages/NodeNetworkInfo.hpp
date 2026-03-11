#pragma once
#include <string>

struct NodeNetworkInfo
{
    int nodeId;
    std::string ip_address;
    int port;

    NodeNetworkInfo()
    {
        nodeId = -1;
        ip_address = "";
        port = -1;
    }

    NodeNetworkInfo(int nId, std::string ip, int p)
    {
        nodeId = nId;
        ip_address = ip;
        port = p;
    }
};