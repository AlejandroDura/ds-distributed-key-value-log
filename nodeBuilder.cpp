#include <iostream>
#include <string>
#include "node/node.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

bool isValidIp(const std::string &ip)
{
    sockaddr_in addr{};

    return inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) == 1;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Invalid argument number" << std::endl;
        return 1;
    }

    bool leader;
    int nodePort;
    vector<string> peers = {"127.0.0.1:5000", "127.0.0.1:5001", "127.0.0.1:5002"};

    // Get node type
    std::string nodeType = argv[1];
    if (nodeType != "leader" && nodeType != "follower")
    {
        std::cerr << "Invalid node type" << std::endl;
        return 1;
    }

    // Get ip
    std::string nodeIp = argv[2];
    if (!isValidIp(nodeIp))
    {
        std::cerr << "Invalid node ip" << std::endl;
        return 1;
    }

    // Get port number
    try
    {
        nodePort = std::stoi(argv[3]);
    }
    catch (const std::invalid_argument &)
    {
        std::cerr << "Invalid port\n";
        return 1;
    }
    catch (const std::out_of_range &)
    {
        std::cerr << "Port out of range\n";
        return 1;
    }

    // Get if it is leader or not
    leader = nodeType == "leader";

    // Create node
    Node *node = new Node(leader, nodeIp, nodePort, peers);

    // Start node
    if (node)
    {
        node->run();
    }
}