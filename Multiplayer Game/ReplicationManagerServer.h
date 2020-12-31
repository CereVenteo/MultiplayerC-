#pragma once
#include <unordered_map>

// TODO(you): World state replication lab session
enum class ReplicationAction
{
	None, Create, Update, Destroy
};

struct ReplicationCommand
{
	ReplicationAction action;
	uint32 networkId;
};

class ReplicationManagerServer
{
public:
	void create(uint32 networkId);
	void update(uint32 networkId);
	void destroy(uint32 networkId);

	void write(OutputMemoryStream& packet);




	//More members...


	//array de acciones con ids.
	std::unordered_map<uint32,ReplicationCommand> replications_map;
	
};