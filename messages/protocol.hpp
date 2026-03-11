#pragma once
#include <string>
#include "register/log.hpp"
#include "message_type.hpp"
#include "NodeNetworkInfo.hpp"
#include "message.hpp"

using namespace std;

class Protocol
{
public:
    static string serialize(MessageType type, const Message &msgData);
    static Message deserialize(const string &msg);
    static MessageType parseType(const string &msg);
    static string CreateReplicationMessage(int index, string key, string value);
    static NodeNetworkInfo deserializeNodeNetworkInfo(const string &networkData);
};