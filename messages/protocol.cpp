#include "protocol.hpp"
#include <sstream>
#include <iostream>
#include "utils/logger.hpp"

using namespace std;

string Protocol::CreateReplicationMessage(int index, string key, string value)
{
    stringstream ss;

    ss << "REPLICATION" << "|" << index << "|" << key << "|" << value;

    return ss.str();
}

string Protocol::serialize(MessageType type, const LogEntry &entry)
{
    stringstream ss;

    switch (type)
    {
    case MessageType::CLIENT_SET:
        ss << "CLIENT_SET" << "|" << entry.key << "|" << entry.value;
        break;

    case MessageType::REPLICATION:
        ss << "REPLICATION" << "|" << entry.index << "|" << entry.key << "|" << entry.value;
        break;

    case MessageType::ACK_NLE:
        ss << "ACK_NLE" << "|" << entry.index;
        break;

    default:
        ss << "UNKNOWN";
        break;
    }

    return ss.str();
}

LogEntry Protocol::deserialize(const string &msg)
{
    stringstream ss(msg);
    string token;
    LogEntry entry;

    entry.index = -1;

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

        entry.key = token;

        // Value
        if (!getline(ss, token, '|'))
        {
            break;
        }

        entry.value = token;
        entry.isValid = true;
        break;
    case MessageType::REPLICATION:
        // Message type
        if (!getline(ss, token, '|'))
        {
            break;
        }

        // Index
        if (!getline(ss, token, '|'))
        {
            break;
        }

        entry.index = stoi(token);

        // Key
        if (!getline(ss, token, '|'))
        {
            break;
        }

        entry.key = token;

        // Value
        if (!getline(ss, token, '|'))
        {
            break;
        }

        entry.value = token;
        entry.isValid = true;
        break;
    case MessageType::ACK_NLE:
        // Message type
        if (!getline(ss, token, '|'))
        {
            break;
        }

        // Index
        if (!getline(ss, token, '|'))
        {
            break;
        }

        entry.index = stoi(token);
        entry.isValid = true;
        break;
    }

    return entry;
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