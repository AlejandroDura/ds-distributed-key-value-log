#pragma once
#include <thread>
#include "node/node.hpp"
#include "messages/NodeNetworkInfo.hpp"
#include <arpa/inet.h>

class Server
{
public:
    Server(int port, Node *node);
    void start();

private:
    int port;
    Node *node;

    void handleClient(int clientSocket, const NodeNetworkInfo &clientNetworkInfo);
    NodeNetworkInfo getInPeerNetworkInfo(const sockaddr_in &inPeerAddress);
};