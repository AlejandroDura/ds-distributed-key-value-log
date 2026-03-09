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
    Node(bool isLeader, int port, const vector<string> &peerList);
    void processMessage(const string &msg);
    void printState();
    void printLog();
    void run();

private:
    bool isRunning;
    Server *server;

    bool leader;
    vector<string> peers;

    unordered_map<string, string> data;
    vector<LogEntry> log;
    mutex dataMutex;
    mutex ackMutex;

    unordered_map<int, int> ack_new_log_entry;

    void applyOperation(const LogEntry &entry);
    void replicateToFollowers(const LogEntry &entry);
    void sendToPeer(const string &peerNetworkData, const string &messageToSend);
    int openConnectionWithPeer(const NodeNetworkInfo &peerNetworkInfo);
    void sendAck();
    void handleAck(const string &ackMessage);

    // Checks
    void checkInconsistency();
};