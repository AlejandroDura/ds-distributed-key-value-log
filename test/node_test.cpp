#include <iostream>
#include "messages/protocol.hpp"
#include "node/node.hpp"

using namespace std;

int main()
{
    Node leader(true);
    Node follower(false);

    LogEntry entry{0, "x", "10"};

    // Client sends to leader
    cout << "------CLIENT SENDS TO LEADER-----" << endl;
    string msg = Protocol::serialize(MessageType::CLIENT_SET, entry);
    leader.processMessage(msg);

    // Leader replicates to follower
    cout << "------LEADER REPLICATES TO FOLLOWER-----" << endl;
    string repl = Protocol::serialize(MessageType::REPLICATION, entry);
    follower.processMessage(repl);

    cout << "Leader state:" << endl;
    leader.printState();

    cout << "Follower state:" << endl;
    follower.printState();
}