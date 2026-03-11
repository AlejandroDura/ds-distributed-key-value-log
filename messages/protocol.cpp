#include "protocol.hpp"
#include <sstream>
#include <iostream>
#include "utils/logger.hpp"

using namespace std;

// This function is deprecated
string Protocol::CreateReplicationMessage(int index, string key, string value)
{
    stringstream ss;

    ss << "REPLICATION" << "|" << index << "|" << key << "|" << value;

    return ss.str();
}

string Protocol::serialize(MessageType type, const Message &msgData)
{
    stringstream ss;

    switch (type)
    {
    case MessageType::CLIENT_SET:
        ss << "CLIENT_SET" << "|" << msgData.key << "|" << msgData.value;
        break;

    case MessageType::REPLICATION:
        ss << "REPLICATION" << "|" << msgData.nodeId << "|" << msgData.logIndex << "|" << msgData.key << "|" << msgData.value;
        break;

    case MessageType::ACK_NLE:
        ss << "ACK_NLE" << "|" << msgData.nodeId << "|" << msgData.logIndex;
        break;

    default:
        ss << "UNKNOWN";
        break;
    }

    return ss.str();
}

Message Protocol::deserialize(const string &msg)
{
    stringstream ss(msg);
    string token;
    Message messageData;

    MessageType messageType = parseType(msg);

    switch (messageType)
    {
    case MessageType::CLIENT_SET:
        // Message type
        if (!getline(ss, token, '|'))
        {
            break;
        }

        // Key
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.key = token;

        // Value
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.value = token;
        messageData.isValid = true;
        break;
    case MessageType::REPLICATION:
        // Message type
        if (!getline(ss, token, '|'))
        {
            break;
        }

        // Node id
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.nodeId = stoi(token);

        // Index
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.logIndex = stoi(token);

        // Key
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.key = token;

        // Value
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.value = token;
        messageData.isValid = true;
        break;
    case MessageType::ACK_NLE:
        // Message type
        if (!getline(ss, token, '|'))
        {
            break;
        }

        // Node id
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.nodeId = stoi(token);

        // Index
        if (!getline(ss, token, '|'))
        {
            break;
        }

        messageData.logIndex = stoi(token);
        messageData.isValid = true;
        break;
    }

    return messageData;
}

MessageType Protocol::parseType(const string &msg)
{
    if (msg.rfind("CLIENT_SET", 0) == 0)
        return MessageType::CLIENT_SET;

    if (msg.rfind("REPLICATION", 0) == 0)
        return MessageType::REPLICATION;

    if (msg.rfind("ACK_NLE", 0) == 0)
        return MessageType::ACK_NLE;

    return MessageType::UNKNOWN;
}

NodeNetworkInfo Protocol::deserializeNodeNetworkInfo(const string &networkData)
{
    NodeNetworkInfo info;
    size_t pos = networkData.find(':');

    if (pos == string::npos)
    {
        LOG_ERROR("Invalid node address format");
        return info;
    }

    string ip = networkData.substr(0, pos);
    int port = stoi(networkData.substr(pos + 1));

    info.ip_address = ip;
    info.port = port;

    return info;
}