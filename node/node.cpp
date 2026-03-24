#include "node.hpp"
#include <iostream>
#include "utils/logger.hpp"
#include "messages/server.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <csignal>

Node::Node(bool isLeader, string selfIp, int selfPort, const vector<string> &peerList)
{
    server = new Server(selfIp, selfPort, this);
    leader = isLeader;

    for (int i = 0; i < peerList.size(); i++)
    {
        NodeNetworkInfo peer = Protocol::deserializeNodeNetworkInfo(peerList[i]);
        peer.nodeId = i;
        peers[i] = peer;

        if (peer.ip_address == selfIp && peer.port == selfPort)
        {
            self = peer;
        }
    }
}

void Node::run()
{
    if (!server)
    {
        LOG_ERROR("Node server reference is null");
        return;
    }

    if (isRunning)
    {
        LOG_WARNING("Node is already running");
        return;
    }

    isRunning = true;
    server->start();
}

//-------NODE FUNCTIONS------//

void Node::applyOperation(const LogEntry &entry)
{
    data[entry.key] = entry.value;
    log.push_back(entry);
}

void Node::processMessage(const string &msg, int clientSocket)
{
    Message messageData = Protocol::deserialize(msg);
    MessageType mType = Protocol::parseType(msg);

    if (mType == MessageType::CLIENT_SET)
    {
        LOG_CLIENT_SET("Client write operation triggered");
        if (!leader)
        {
            LOG_ERROR("[NODE] Follower does not accept direct writes");
            return;
        }

        if (messageData.key != "")
        {
            {
                lock_guard<mutex> lock(dataMutex);
                LogEntry newEntry;

                newEntry.key = messageData.key;
                newEntry.value = messageData.value;
                newEntry.index = log.size();

                messageData.logIndex = newEntry.index;
                messageData.nodeId = self.nodeId;

                applyOperation(newEntry);
                addPendingClient(newEntry.index, clientSocket);
                LOG_REPLICATION("[NODE LEADER] Aplying local operation");
            }

            replicateToFollowers(messageData);
        }
    }
    else if (mType == MessageType::REPLICATION)
    {
        if (messageData.nodeId < 0)
        {
            return;
        }

        if (leader)
        {
            LOG_ERROR("[NODE] Leader does not accept replication messages");
            return;
        }

        NodeNetworkInfo msgSenderNetworkInfo = getNodeNetInfo(messageData.nodeId);

        if (messageData.logIndex >= 0 && messageData.key != "")
        {
            {
                lock_guard<mutex> lock(dataMutex);
                LogEntry newEntry;
                newEntry.index = messageData.logIndex;
                newEntry.key = messageData.key;
                newEntry.value = messageData.value;
                applyOperation(newEntry);
                LOG_REPLICATION("[NODE FOLLOWER] Aplying local operation");
            }

            sendAckToLeader(messageData.logIndex, msgSenderNetworkInfo);
        }
    }
    else if (mType == MessageType::ACK_NLE)
    {
        if (leader)
        {
            handleAck(msg);
        }
    }
    else
    {
        cout << "ERROR: Unknown message " << msg << endl;
    }

    printLog();
    printState();
}

void Node::replicateToFollowers(const Message &messageData)
{
    string msg = Protocol::serialize(MessageType::REPLICATION, messageData);
    LOG_NODE("Replication message: " + msg);
    for (int i = 0; i < peers.size(); i++)
    {
        if (peers[i].nodeId == self.nodeId)
        {
            continue;
        }

        LOG_BROADCAST("ip: " + peers[i].ip_address + " port: " + to_string(peers[i].port));
        sendToPeer(peers[i], msg);
    }
}

int Node::openConnectionWithPeer(const NodeNetworkInfo &peerNetworkInfo)
{
    // Create TCP client
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        LOG_ERROR("[NODE] Error creating client socket descriptor");
        return -1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(peerNetworkInfo.port);

    if (inet_pton(AF_INET, peerNetworkInfo.ip_address.c_str(), &serv_addr.sin_addr) <= 0)
    {
        LOG_ERROR("[NODE] Invalid server peer address");
        return -1;
    }

    if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        LOG_ERROR("[NODE] Failed connecting to the server peer");
        close(sock);
        return -1;
    }

    return sock;
}

void Node::sendToPeer(const NodeNetworkInfo &peerNetworkInfo, const string &messageToSend)
{
    int sock = openConnectionWithPeer(peerNetworkInfo);

    if (sock > 0)
    {
        send(sock, messageToSend.c_str(), messageToSend.size(), 0);
        close(sock);
    }
}

void Node::sendAckToLeader(int entryIndex, const NodeNetworkInfo &leaderNetworkInfo)
{
    Message messageData;
    messageData.logIndex = entryIndex;
    messageData.nodeId = self.nodeId;

    string msg = Protocol::serialize(MessageType::ACK_NLE, messageData);
    sendToPeer(leaderNetworkInfo, msg);

    LOG_NODE("[FOLLOWER] sending ACK to the leader, ip: " + leaderNetworkInfo.ip_address + "port: " + to_string(leaderNetworkInfo.port));
}

void Node::handleAck(const string &ackMessage)
{
    Message messageData = Protocol::deserialize(ackMessage);
    int logIndex = messageData.logIndex;
    int senderId = messageData.nodeId;

    if (messageData.isValid && logIndex >= 0)
    {
        {
            lock_guard<mutex> ack(ackMutex);
            newLogEntryConfirmations[logIndex].insert(senderId);

            if (newLogEntryConfirmations[logIndex].size() == 1)
            {
                LOG_NODE("[LEADER] Confirmed ACK for entry from at leas one follower: " + to_string(logIndex));
                // commit entry/write to the client.
                newLogEntryConfirmations.erase(logIndex); // Posible loop over all peers confirmation if we reset here.
                commitClient(logIndex);
            }
        }
    }
}

void Node::newRequest(int clientSocket, const string &msg)
{
    processMessage(msg, clientSocket);
    MessageType messageType = Protocol::parseType(msg);

    if (leader)
    {
        if (messageType != MessageType::CLIENT_SET)
        {
            close(clientSocket);
        }
    }
    else
    {
        close(clientSocket);
    }
}

void Node::addPendingClient(int logIndex, int clientSocket)
{
    lock_guard<mutex> pClients(pendingClientsMutex);
    pendingClients[logIndex] = clientSocket;
    pendingLogEntries[logIndex] = true;
    LOG_WARNING("Added a new client pending");
}

void Node::removePendingClient(int logIndex, int clientSocket)
{
    lock_guard<mutex> pClients(pendingClientsMutex);
    pendingClients.erase(logIndex);
    pendingLogEntries.erase(logIndex);
}

void Node::commitClient(int logIndex)
{
    int clientSocket = -1;

    {
        lock_guard<mutex> pClients(pendingClientsMutex);

        auto it = pendingClients.find(logIndex);
        if (it == pendingClients.end())
        {
            LOG_WARNING("no pending clients find");
            return;
        }

        clientSocket = pendingClients[logIndex];
        if (clientSocket < 0)
        {
            LOG_WARNING("Client socket is invalid");
            return;
        }

        auto it_2 = pendingLogEntries.find(logIndex);
        if (it_2 == pendingLogEntries.end())
        {
            LOG_WARNING("Pending log entries not found");
            return;
        }

        if (!pendingLogEntries[logIndex])
        {
            LOG_WARNING("No pending entry for that entry");
            return;
        }

        pendingClients.erase(logIndex);
        pendingLogEntries.erase(logIndex);
    }

    string msg = "ACK COMMIT OK!";

    int send_res = send(clientSocket, msg.c_str(), msg.size(), 0);
    if (send_res < 0)
    {
        LOG_WARNING("Client disconected before commit");
    }

    close(clientSocket);
}

////////////////
// Getters ////
//////////////

NodeNetworkInfo Node::getNodeNetInfo(int nodeId)
{
    if (nodeId < 0 || nodeId > peers.size() - 1)
    {
        return NodeNetworkInfo();
    }

    return peers[nodeId];
}

NodeNetworkInfo getNodeNetInfo(int nodeId);
////////////////
// Debugging///
//////////////

void Node::printState()
{
    lock_guard<mutex> lock(dataMutex);

    for (const auto &[k, v] : data)
    {
        LOG_STATE(k + "=" + v);
    }
}

void Node::printLog()
{
    for (int i = 0; i < log.size(); i++)
    {
        LogEntry entry = log[i];
        string msg = to_string(entry.index) + "|" + entry.key + "|" + entry.value;
        LOG_CUSTOM("LOG", msg);
    }

    checkInconsistency();
}

void Node::checkInconsistency()
{
    if (log.size() < 2)
    {
        return;
    }

    int lastIndex = log.size() - 1;

    LogEntry currentEntry = log[lastIndex];
    LogEntry prevEntry = log[lastIndex - 1];

    if (prevEntry.index + 1 != currentEntry.index)
    {
        LOG_ERROR("[NODE] Log inconsistency detected");
        std::raise(SIGINT);
    }
}