#include "Networks.h"
#include "ModuleGameObject.h"

bool ModuleGameObject::init()
{
	return true;
}

bool ModuleGameObject::preUpdate()
{
	BEGIN_TIMED_BLOCK(GOPreUpdate);

	static const GameObject::State gNextState[] = {
		GameObject::NON_EXISTING, // After NON_EXISTING
		GameObject::STARTING,     // After INSTANTIATE
		GameObject::UPDATING,     // After STARTING
		GameObject::UPDATING,     // After UPDATING
		GameObject::DESTROYING,   // After DESTROY
		GameObject::NON_EXISTING  // After DESTROYING
	};

	for (GameObject &gameObject : gameObjects)
	{
		gameObject.state = gNextState[gameObject.state];
	}

	END_TIMED_BLOCK(GOPreUpdate);

	return true;
}

bool ModuleGameObject::update()
{
	// Delayed destructions
	for (DelayedDestroyEntry &destroyEntry : gameObjectsWithDelayedDestruction)
	{
		if (destroyEntry.object != nullptr)
		{
			destroyEntry.delaySeconds -= Time.deltaTime;
			if (destroyEntry.delaySeconds <= 0.0f)
			{
				Destroy(destroyEntry.object);
				destroyEntry.object = nullptr;
			}
		}
	}

	return true;
}

bool ModuleGameObject::postUpdate()
{
	return true;
}

bool ModuleGameObject::cleanUp()
{
	return true;
}

GameObject * ModuleGameObject::Instantiate()
{
	for (uint32 i = 0; i < MAX_GAME_OBJECTS; ++i)
	{
		GameObject &gameObject = App->modGameObject->gameObjects[i];

		if (gameObject.state == GameObject::NON_EXISTING)
		{
			gameObject = GameObject();
			gameObject.id = i;
			gameObject.state = GameObject::INSTANTIATE;
			return &gameObject;
		}
	}

	ASSERT(0); // NOTE(jesus): You need to increase MAX_GAME_OBJECTS in case this assert crashes
	return nullptr;
}

void ModuleGameObject::Destroy(GameObject * gameObject)
{
	ASSERT(gameObject->networkId == 0); // NOTE(jesus): If it has a network identity, it must be destroyed by the Networking module first

	static const GameObject::State gNextState[] = {
		GameObject::NON_EXISTING, // After NON_EXISTING
		GameObject::DESTROY,      // After INSTANTIATE
		GameObject::DESTROY,      // After STARTING
		GameObject::DESTROY,      // After UPDATING
		GameObject::DESTROY,      // After DESTROY
		GameObject::DESTROYING    // After DESTROYING
	};

	ASSERT(gameObject->state < GameObject::STATE_COUNT);
	gameObject->state = gNextState[gameObject->state];
}

void ModuleGameObject::Destroy(GameObject * gameObject, float delaySeconds)
{
	for (uint32 i = 0; i < MAX_GAME_OBJECTS; ++i)
	{
		if (App->modGameObject->gameObjectsWithDelayedDestruction[i].object == nullptr)
		{
			App->modGameObject->gameObjectsWithDelayedDestruction[i].object = gameObject;
			App->modGameObject->gameObjectsWithDelayedDestruction[i].delaySeconds = delaySeconds;
			break;
		}
	}
}

GameObject * Instantiate()
{
	GameObject *result = ModuleGameObject::Instantiate();
	return result;
}

void Destroy(GameObject * gameObject)
{
	ModuleGameObject::Destroy(gameObject);
}

void Destroy(GameObject * gameObject, float delaySeconds)
{
	ModuleGameObject::Destroy(gameObject, delaySeconds);
}

void GameObject::WCreate(OutputMemoryStream& packet)
{
	packet << this->position.x;
	packet << this->position.y;
	packet << this->size.x;
	packet << this->size.y;
	packet << this->angle;

	if (this->sprite)
	{
		packet << true;
		sprite->Write(packet);
	}
	else
		packet << false;


	if (this->collider)
	{
		packet << true;
		packet << collider->type;
		packet << collider->isTrigger;
	}
	else
		packet << false;
	
	if (this->behaviour)
	{
		packet << true;
		packet << this->behaviour->type();
		behaviour->write(packet);
	}
	else
		packet << false;
	
}

void GameObject::RCreate(const InputMemoryStream& packet)
{
	packet >> this->position.x;
	packet >> this->position.y;
	packet >> this->size.x;
	packet >> this->size.y;
	packet >> this->angle;

	init_pos = final_pos = position;
	init_ang = final_ang = angle;

	bool ret = false;
	packet >> ret;
	if (ret)
	{
		sprite = App->modRender->addSprite(this);
		sprite->Read(packet);
	}
	packet >> ret;
	if (ret)
	{
		ColliderType type = ColliderType::None;
		packet >> type;
		collider = App->modCollision->addCollider(type, this);
		packet >> this->collider->isTrigger;
	}
	packet >> ret;
	if (ret)
	{
		BehaviourType type = BehaviourType::None;
		packet >> type;
		behaviour = App->modBehaviour->addBehaviour(type, this);
		behaviour->read(packet);
	}
}

void GameObject::WUpdate(OutputMemoryStream& packet)
{
	packet << this->position.x;
	packet << this->position.y;
	packet << this->angle;

	if (this->sprite)
	{
		packet << true;
		sprite->Ca_w(packet);
	}
	else
		packet << false;


	if (this->behaviour)
	{
		packet << true;
		behaviour->write(packet);
	}
	else
		packet << false;
	
}

void GameObject::RUpdate(const InputMemoryStream& packet)
{
	if (networkInterpolationEnabled)
	{
		//Restart our lerps
		seconds_elapsed = 0.0f;
		init_pos = position;
		init_ang = angle;
		packet >> final_pos.x;
		packet >> final_pos.y;
		packet >> final_ang;

		bool ret = false;
		packet >> ret;
		if (ret)
		{
			sprite->Re_w(packet);
		}

		packet >> ret;
		if (ret)
			behaviour->read(packet);
		
	}
	else
	{
		packet >> position.x;
		packet >> position.y;
		packet >> angle;

		bool ret = false;
		packet >> ret;
		if (ret)
		{
			sprite->Re_w(packet);
		}

		packet >> ret;
		if (ret)
			behaviour->read(packet);
		
	}
}

void GameObject::Interpolation()
{
	float time = seconds_elapsed / REPLICATION_SECONDS;

	if (time < 1)
	{
		position = lerp(init_pos, final_pos, time);
		angle = lerp(init_ang, final_ang, time);
		seconds_elapsed += Time.deltaTime;
	}
}
