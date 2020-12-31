#include "Networks.h"
#include "ReplicationManagerServer.h"
// TODO(you): World state replication lab session

void ReplicationManagerServer::create(uint32 networkId)
{
	// maybe upgrade this <3 gg
	ReplicationCommand helper;
	helper.action = ReplicationAction::Create;
	helper.networkId = networkId;

	replications_map.insert(std::pair<uint32,ReplicationCommand>(networkId, helper));
}

void ReplicationManagerServer::update(uint32 networkId)
{
	replications_map[networkId].action = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	replications_map[networkId].action = ReplicationAction::Destroy;
}

void ReplicationManagerServer::write(OutputMemoryStream& packet)
{
	for (auto it = replications_map.begin(); it != replications_map.end(); ++it)
	{
		packet << it->second.networkId;
		packet << it->second.action;

		switch (it->second.action)
		{
		case ReplicationAction::None:
		{

		}
		break;
		case ReplicationAction::Create:
		{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(it->second.networkId);
			gameObject->WCreate(packet);
		}
		break;
		case ReplicationAction::Update:
		{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(it->second.networkId);
			gameObject->WUpdate(packet);
		}
		break;
		case ReplicationAction::Destroy:
		{

		}
		break;
		default:
			break;
		}

		it->second.action = ReplicationAction::None;
	}
}
