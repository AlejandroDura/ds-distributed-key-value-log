#pragma once
#include <thread>
#include "node/node.hpp"
#include "messages/NodeNetworkInfo.hpp"

class Server
{
public:
    Server(int port, Node *node);
    void start();

private:
    int port;
    Node *node;

    void handleClient(int clientSocket);
    NodeNetworkInfo getInPeerNetworkInfo(const sockaddr_in &peerAddress);
};