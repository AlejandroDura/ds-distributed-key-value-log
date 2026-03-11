#pragma once
#include <thread>
#include "node/node.hpp"
#include "messages/NodeNetworkInfo.hpp"
#include <arpa/inet.h>

class Server
{
public:
    Server(string address, int port, Node *node);
    void start();

private:
    int port;
    string ip;
    Node *node;

    void handleClient(int clientSocketDesc);
};