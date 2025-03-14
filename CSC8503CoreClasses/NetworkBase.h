#pragma once
//#include "./enet/enet.h"
struct _ENetHost;
struct _ENetPeer;
struct _ENetEvent;

enum BasicNetworkMessages {
	None,
	Hello,
	Message,
	String_Message,
	Delta_State,	//1 byte per channel since the last state
	Full_State,		//Full transform etc
	Received_State, //received from a client, informs that its received packet n
	Player_Connected,
	Player_Disconnected,
	Player_ID_Assignment,
	Player_Position,
	Shutdown
};

struct GamePacket {
	short size;
	short type;

	GamePacket() {
		type		= BasicNetworkMessages::None;
		size		= 0;
	}

	GamePacket(short type) : GamePacket() {
		this->type	= type;
	}

	int GetTotalSize() {
		return sizeof(GamePacket) + size;
	}
};

struct PlayerIDPacket : public GamePacket {
	int playerID;

	PlayerIDPacket(int id) {
		type = Player_ID_Assignment;
		size = sizeof(PlayerIDPacket) - sizeof(GamePacket);
		playerID = id;
	}
};

struct PlayerPositionPacket : public GamePacket {
	int playerID;
	float posX;
	float posY;
	float posZ;

	PlayerPositionPacket(int id, float x, float y, float z) {
		type = Player_Position;
		size = sizeof(PlayerPositionPacket) - sizeof(GamePacket);
		playerID = id;
		posX = x;
		posY = y;
		posZ = z;
	}
};

struct StringPacket : public GamePacket {
	char stringData[256];

	StringPacket(const std::string& message) {
		type = BasicNetworkMessages::String_Message;
		size = (short)message.length();
		memcpy(stringData, message.data(), size);
	}

	std::string GetStringFromData() {
		std::string realstring(stringData, size);
		return realstring;
	}
};


class PacketReceiver {
public:
	virtual void ReceivePacket(int type, GamePacket* payload, int source = -1) = 0;
};

class NetworkBase	{
public:
	static void Initialise();
	static void Destroy();

	static int GetDefaultPort() {
		return 1234;
	}

	void RegisterPacketHandler(int msgID, PacketReceiver* receiver) {
		packetHandlers.insert(std::make_pair(msgID, receiver));
	}

	_ENetHost* netHandle;

protected:
	NetworkBase();
	~NetworkBase();

	bool ProcessPacket(GamePacket* p, int peerID = -1);

	typedef std::multimap<int, PacketReceiver*>::const_iterator PacketHandlerIterator;

	bool GetPacketHandlers(int msgID, PacketHandlerIterator& first, PacketHandlerIterator& last) const {
		auto range = packetHandlers.equal_range(msgID);

		if (range.first == packetHandlers.end()) {
			return false; //no handlers for this message type!
		}
		first	= range.first;
		last	= range.second;
		return true;
	}

	std::multimap<int, PacketReceiver*> packetHandlers;
};

class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(std::string name)
	{
		this->name = name;
	}
	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message)
		{
			StringPacket* realPacket = (StringPacket*)payload;
			std::string message = realPacket->GetStringFromData();
			std::cout << name << " received message: " << message << std::endl;
		}
	}
protected:
	std::string name;
};