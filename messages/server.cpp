#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include "utils/logger.hpp"

using namespace std;

Server::Server(string address, int p, Node *n) : ip(address), port(p), node(n) {} // Añadri la IP como escucha. Quitar lo de address.sin_addr.s_addr = INADDR_ANY;

void Server::start()
{
    // Create server socket descriptor file
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFd < 0)
    {
        LOG_ERROR("Server: Server descriptor creation failed");
        return;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // To avoid TIME_WAIT after the server or node is shot down.

    sockaddr_in address{};
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
    address.sin_port = htons(port);

    // Bind server socket descriptor with port number
    if (bind(serverFd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        LOG_ERROR("Server: Socket binding error");
        return;
    }

    // Server listening
    listen(serverFd, 10);

    LOG_SUCCESS("Server initialized > listening on port: " + to_string(ntohs(address.sin_port)));
    // Accept and dispatch client peer conections loop
    while (true)
    {
        sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);

        int clientSocketDesc = accept(serverFd, (sockaddr *)&clientAddr, &len);

        if (clientSocketDesc < 0)
        {
            LOG_ERROR("Server: Error client accept conection");
            continue;
        }

        thread t(&Server::handleClient, this, clientSocketDesc);
        t.detach();
    }
}

void Server::handleClient(int clientSocketDesc)
{
    if (!node)
    {
        LOG_ERROR("Node reference null");
        close(clientSocketDesc);
        return;
    }

    char buffer[1024] = {0};
    int bytesRead = read(clientSocketDesc, buffer, 1024);

    if (bytesRead > 0)
    {
        string msg(buffer, bytesRead);
        // node->processMessage(msg);
        node->newRequest(clientSocketDesc, msg);
    }

    // close(clientSocketDesc);
}