#include <iostream>
#include <string>
#include "node/node.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Invalid argument number" << std::endl;
        return 1;
    }

    bool leader;
    int port;
    vector<string> peers = {"127.0.0.1:5000", "127.0.0.1:5001", "127.0.0.1:5002"};

    std::string nodeType = argv[1];
    if (nodeType != "leader" && nodeType != "follower")
    {
        std::cerr << "Invalid node type" << std::endl;
        return 1;
    }

    // Get port number
    try
    {
        port = std::stoi(argv[2]);
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
    Node *node = new Node(leader, port, peers);

    // Start node
    if (node)
    {
        node->run();
    }
}