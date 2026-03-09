#pragma once
#include <string>
#include "register/log.hpp"
#include "message_type.hpp"
#include "NodeNetworkInfo.hpp"

using namespace std;

class Protocol
{
public:
    static string serialize(MessageType type, const LogEntry &entry);
    static LogEntry deserialize(const string &msg);
    static MessageType parseType(const string &msg);
    static string CreateReplicationMessage(int index, string key, string value);
    static NodeNetworkInfo deserializeNodeNetworkInfo(const string &networkData);
};