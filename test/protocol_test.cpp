#include <iostream>
#include "messages/protocol.hpp"

using namespace std;

int main()
{
    MessageType messageType;
    LogEntry entry{0, "x", "10"};
    string message;

    // Create replication message
    cout << "REPLICATION MESSAGE:" << endl;
    string replicationMessage = Protocol::CreateReplicationMessage(5, "x", "4");
    cout << "Replication message: " << replicationMessage << endl
         << endl;

    // Serialize CLIENT_SET
    cout << "SERIALIZE CLIENT_SET:" << endl;
    messageType = MessageType::CLIENT_SET;

    string serializedClientSet = Protocol::serialize(messageType, entry);

    cout << "Serialized message: " << serializedClientSet << endl
         << endl;

    // Serialize REPLICATION
    cout << "SERIALIZE REPLICATION:" << endl;
    messageType = MessageType::REPLICATION;

    string serializedReplication = Protocol::serialize(messageType, entry);

    cout << "Serialized message: " << serializedReplication << endl
         << endl;

    // Deserialize CLIENT_SET
    cout << "DESERIALIZE CLIENT_SET:" << endl;
    message = "CLIENT_SET|x|20";
    LogEntry deserializedClientSet = Protocol::deserialize(message);

    cout << "Index: " << deserializedClientSet.index << endl;
    cout << "Key: " << deserializedClientSet.key << endl;
    cout << "Value: " << deserializedClientSet.value << endl
         << endl;

    // Deserialize REPLICATION
    cout << "DESERIALIZE REPLICATION:" << endl;
    message = "REPLICATION|2|x|20";
    LogEntry deserializedReplication = Protocol::deserialize(message);

    cout << "Index: " << deserializedReplication.index << endl;
    cout << "Key: " << deserializedReplication.key << endl;
    cout << "Value: " << deserializedReplication.value << endl
         << endl;
}