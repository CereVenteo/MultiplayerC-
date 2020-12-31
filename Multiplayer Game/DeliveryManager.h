#pragma once

#include <list>

class DeliveryManager;
class ReplicationManagerServer;
struct ReplicationCommand;

class DeliveryDelegate 
{
public:
    virtual void onDeliverySuccess(DeliveryManager* deliveryManager) = 0;
    virtual void onDeliveryFailure(DeliveryManager* deliveryManager) = 0;
};

class ReplicationDeliveryDelegate : public DeliveryDelegate
{
public:

    ReplicationDeliveryDelegate(ReplicationManagerServer* repManager);

    void onDeliverySuccess(DeliveryManager* deliverManager)
    {

    }
    void onDeliveryFailure(DeliveryManager* deliverManager)
    {
        repeatReplication();
    }

private:
    void repeatReplication();

    std::vector<ReplicationCommand> replications;
    ReplicationManagerServer* replication_manager = nullptr;
};

struct Delivery 
{
    uint32 sequenceNumber = 0;
    double dispatchTime = 0.0;
    DeliveryDelegate* delegate = nullptr;
};

class DeliveryManager 
{
public:

    DeliveryManager() {};
    ~DeliveryManager() { clear(); };

    // For senders to write a new sequence number into a packet
    Delivery* writeSequenceNumber(OutputMemoryStream& packet);
    // For receivers to process the sequence number from an incoming packet
    bool processSequenceNumber(const InputMemoryStream& packet);

    // For receivers to write ack'ed sequence numbers into a packet
    bool hasSequenceNumbersPendingAck() const;
    void writeSequenceNumbersPendingAck(OutputMemoryStream& packet);

    // For senders to process ack'ed numbers from a packet
    void processAckSequenceNumbers(const InputMemoryStream& packet);
    void processTimedOutPackets();

    void clear();

private:
    // Sender side
    uint32 next_outgoing_sequence_number = 0;
    std::list<Delivery*> pending_deliveries;

    // Receiver side
    uint32 next_expected_sequence_number = 0;
    std::list<uint32> pending_acks;
};