#pragma once
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>
#include "messages/protocol.hpp"
#include "register/log.hpp"
#include "messages/message_type.hpp"
#include "messages/NodeNetworkInfo.hpp"

using namespace std;

class Server;

class Node
{
public:
    Node(bool isLeader, string selfIp, int selfPort, const vector<string> &peerList);
    void processMessage(const string &msg);
    void printState();
    void printLog();
    void run();

private:
    bool isRunning;
    Server *server;

    bool leader;
    NodeNetworkInfo self;
    // vector<string> peers;
    unordered_map<int, NodeNetworkInfo> peers;

    unordered_map<string, string>
        data;
    vector<LogEntry> log;
    mutex dataMutex;
    mutex ackMutex;

    unordered_map<int, int> ackNewLogEntry;

    void applyOperation(const LogEntry &entry);
    void replicateToFollowers(const Message &messageData);
    void sendToPeer(const NodeNetworkInfo &peerNetworkInfo, const string &messageToSend);
    int openConnectionWithPeer(const NodeNetworkInfo &peerNetworkInfo);
    void sendAckToLeader(int entryIndex, const NodeNetworkInfo &leaderNetworkInfo);
    void handleAck(const string &ackMessage);

    // gets
    NodeNetworkInfo getNodeNetInfo(int nodeId);

    // Checks
    void checkInconsistency();
};