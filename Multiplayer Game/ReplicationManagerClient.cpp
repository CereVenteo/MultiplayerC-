#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
{
	while ((int)packet.RemainingByteCount() > 0)
	{
		uint32 networkId = 0;
		packet >> networkId;

		ReplicationAction action = ReplicationAction::None;
		packet >> action;

		switch (action)
		{
		case ReplicationAction::Create:
		{
			GameObject* gameObject = App->modGameObject->Instantiate();

			if (App->modLinkingContext->getNetworkGameObject(networkId) != nullptr)
			{
				gameObject->RCreate(packet);
				App->modGameObject->Destroy(gameObject);
			}
			else
			{
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, networkId);
				gameObject->RCreate(packet);

				if (networkId == App->modNetClient->GetNetworkId())
					gameObject->networkInterpolationEnabled = false;
			}

		}
		break;
		case ReplicationAction::Update:
		{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkId);
			gameObject->RUpdate(packet);
		}
		break;
		case ReplicationAction::Destroy:
		{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkId);
			App->modLinkingContext->unregisterNetworkGameObject(gameObject);
			App->modGameObject->Destroy(gameObject);
		}
		break;
		default:
			break;
		}
	}
}
