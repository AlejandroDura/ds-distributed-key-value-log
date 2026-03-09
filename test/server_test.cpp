#include <iostream>
#include "messages/protocol.hpp"
#include "node/node.hpp"
#include "messages/server.hpp"

#include <thread>
#include <chrono>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Invalid amount of arguments\n";
        return 1;
    }

    string messageType = argv[1];
    bool isLeader;

    isLeader = messageType == "CLIENT_SET";
    if (messageType != "CLIENT_SET" && messageType != "REPLICATION")
    {
        cerr << "Invalid message type\n";
        return 1;
    }

    int const SERVER_PORT = 5000;
    vector<string> peers = {"127.0.0.1:5001", "127.0.0.1:5002", "127.0.0.1:5003"};
    // Create node
    Node node(SERVER_PORT, isLeader, peers);

    // Create server
    Server server(SERVER_PORT, &node);

    // Start server in a thread
    thread serverThread([&server]()
                        { server.start(); });

    // Wait a little for the server start
    this_thread::sleep_for(chrono::milliseconds(500));

    // Create TCP client
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cerr << "Server test: Error creating client socket descriptor\n";
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        cerr << "Server test: Invalid server address\n";
        return 1;
    }

    if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cerr << "Server test: Error conecting the client to the server\n";
        return 1;
    }

    // Send message CLIENT_SET from the client to the server
    string msg;
    if (messageType == "CLIENT_SET")
    {
        msg = messageType + "|x|10";
    }

    if (messageType == "REPLICATION")
    {
        msg = messageType + "|0|x|10";
    }

    send(sock, msg.c_str(), msg.size(), 0);

    close(sock);

    // Wait a little for the server proccesing data
    this_thread::sleep_for(chrono::milliseconds(1000));

    // Show node state
    node.printState();

    // serverThread.join();
}