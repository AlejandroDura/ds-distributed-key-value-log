#pragma once
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>
#include "messages/protocol.hpp"
#include "register/log.hpp"
#include "messages/message_type.hpp"
#include "messages/NodeNetworkInfo.hpp"
#include <unordered_set>

using namespace std;

class Server;

class Node
{
public:
    Node(bool isLeader, string selfIp, int selfPort, const vector<string> &peerList);
    void newRequest(int clientSocket, const string &msg);
    void processMessage(const string &msg, int clientSocket);
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
    mutex pendingClientsMutex;

    unordered_map<int, bool> pendingLogEntries;

    /**
     * @note Stores the pending clients, awaiting for a write confirmation in the system. Each operation
     * in the system has an implicit log index.
     * @note Key: logIndex
     * @note Value: The client socket which is waiting for the confirmation of the operation
     * asociated with its log index.
     */
    unordered_map<int, int> pendingClients;

    /**
     * @note Stores a counter for each new log entry confirmation. Each new log entry can register more than one
     * ACK confirmation. One for each peer.
     * @note Key: logIndex
     * @note Value: ACK confirmation counter for a log index.
     */
    unordered_map<int, unordered_set<int>> newLogEntryConfirmations;

    void applyOperation(const LogEntry &entry);
    void replicateToFollowers(const Message &messageData);
    void sendToPeer(const NodeNetworkInfo &peerNetworkInfo, const string &messageToSend);
    int openConnectionWithPeer(const NodeNetworkInfo &peerNetworkInfo);
    void sendAckToLeader(int entryIndex, const NodeNetworkInfo &leaderNetworkInfo);
    void handleAck(const string &ackMessage);
    void commitClient(int logIndex);

    /**
     * @note A client is waiting for its write confirmations. Each write operation has asociated its own
     * log index inside the system
     */
    void addPendingClient(int logIndex, int clientSocket);

    /**
     * @note Remove the pending client. Each pending client is asociated with a log index, which is the index
     * asociated to its write operation.
     */
    void removePendingClient(int logIndex, int clientSocket);

    // gets
    NodeNetworkInfo getNodeNetInfo(int nodeId);

    // Checks
    void checkInconsistency();
};