#include "node.hpp"
#include <iostream>
#include "utils/logger.hpp"
#include "messages/server.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <csignal>

Node::Node(bool isLeader, int port, const vector<string> &peerList)
{
    server = new Server(port, this);
    leader = isLeader;
    peers = peerList;
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

void Node::processMessage(const string &msg)
{
    MessageType mType = Protocol::parseType(msg);

    if (mType == MessageType::CLIENT_SET)
    {
        LOG_CLIENT_SET("Client write operation triggered");
        if (!leader)
        {
            LOG_ERROR("[NODE] Follower does not accept direct writes");
            return;
        }

        LogEntry newEntry = Protocol::deserialize(msg);

        if (newEntry.key != "")
        {
            {
                lock_guard<mutex> lock(dataMutex);
                newEntry.index = log.size();
                applyOperation(newEntry);
                LOG_REPLICATION("[NODE LEADER] Aplying local operation");
            }
            // Broadcast to followers via TCP
            replicateToFollowers(newEntry);
        }
    }
    else if (mType == MessageType::REPLICATION)
    {
        if (leader)
        {
            LOG_ERROR("[NODE] Leader does not accept replication messages");
            return;
        }

        LogEntry newEntry = Protocol::deserialize(msg);

        if (newEntry.index >= 0 && newEntry.key != "")
        {
            {
                lock_guard<mutex> lock(dataMutex);
                applyOperation(newEntry);
                LOG_REPLICATION("[NODE FOLLOWER] Aplying local operation");
            }
        }
    }
    else
    {
        cout << "ERROR: Unknown message " << msg << endl;
    }

    printLog();
    printState();
}

void Node::replicateToFollowers(const LogEntry &entry)
{
    string msg = Protocol::serialize(MessageType::REPLICATION, entry);
    LOG_NODE("Replication message: " + msg);
    for (int i = 0; i < peers.size(); i++)
    {
        LOG_BROADCAST(peers[i]);
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

void Node::sendToPeer(const string &peerNetworkData, const string &messageToSend)
{
    NodeNetworkInfo peerNetworkInfo = Protocol::deserializeNodeNetworkInfo(peerNetworkData);
    int sock = openConnectionWithPeer(peerNetworkInfo);

    if (sock > 0)
    {
        send(sock, messageToSend.c_str(), messageToSend.size(), 0);
        close(sock);
    }
}

void sendAck();
void handleAck(const string &ackMessage);

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