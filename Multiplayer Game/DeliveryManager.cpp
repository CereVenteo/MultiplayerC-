#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session


Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream& packet)
{
    packet.Write(next_outgoing_sequence_number);

    Delivery* delivery = new Delivery();
	delivery->sequenceNumber = next_outgoing_sequence_number++;
	delivery->dispatchTime = Time.time;
    pending_deliveries.push_back(delivery);

    return delivery;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
    uint32 sequence_number = 0;
    packet.Read(sequence_number);
    if (sequence_number >= next_expected_sequence_number) {
        pending_acks.push_back(sequence_number);
		next_expected_sequence_number = sequence_number + 1;
        return true;
    }

    return false;
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
    return !pending_acks.empty();
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream& packet)
{
    if (hasSequenceNumbersPendingAck())
    {
		for (auto sequenceNumber : pending_acks)
		{
			packet.Write(sequenceNumber);
		}
		pending_acks.clear();
    }
}

void DeliveryManager::processAckSequenceNumbers(const InputMemoryStream& packet)
{
	while ((int)packet.RemainingByteCount() > 0)
	{
		uint32 sequenceNumber = 0;
		packet.Read(sequenceNumber);

		for (auto it = pending_deliveries.begin(); it != pending_deliveries.end();)
		{
			if ((*it)->sequenceNumber == sequenceNumber)
			{
				(*it)->delegate->onDeliverySuccess(this);
				delete (*it)->delegate;
				delete* it;
				it = pending_deliveries.erase(it);
			}
			else
				++it;
		}
	}
}

void DeliveryManager::processTimedOutPackets()
{
	for (auto it = pending_deliveries.begin(); it != pending_deliveries.end();)
	{
		if (Time.time - (*it)->dispatchTime >= PACKET_DELIVERY_TIMEOUT_SECONDS)
		{
			(*it)->delegate->onDeliveryFailure(this);
			delete (*it)->delegate;
			delete* it;
			it = pending_deliveries.erase(it);
		}
		else
			++it;
	}
}

void DeliveryManager::clear()
{
	for (auto it = pending_deliveries.begin(); it != pending_deliveries.end();)
	{
		delete (*it)->delegate;
		delete* it;
		it = pending_deliveries.erase(it);
	}
	pending_deliveries.clear();
	pending_acks.clear();
	next_outgoing_sequence_number = 0;
	next_expected_sequence_number = 0;
}

ReplicationDeliveryDelegate::ReplicationDeliveryDelegate(ReplicationManagerServer* repManager) :replication_manager(repManager)
{
	for (std::unordered_map<uint32, ReplicationCommand>::iterator it = replication_manager->replications_map.begin(); it != replication_manager->replications_map.end(); ++it)
	{
		replications.push_back((*it).second);
	}
}

void ReplicationDeliveryDelegate::repeatReplication()
{
	for (std::vector<ReplicationCommand>::iterator it = replications.begin(); it != replications.end(); ++it)
	{
		switch ((*it).action)
		{
		case ReplicationAction::Create:
			replication_manager->create((*it).networkId);
			break;
		case ReplicationAction::Update:
			replication_manager->update((*it).networkId);
			break;
		case ReplicationAction::Destroy:
			replication_manager->destroy((*it).networkId);
			break;
		default:
			break;
		}
	}
}
