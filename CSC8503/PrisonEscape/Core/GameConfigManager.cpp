#include "GameConfigManager.h"

using namespace NCL;
using namespace CSC8503;

GameConfigManager::GameConfigManager()
{
}

GameConfigManager::~GameConfigManager()
{
	if (networkConfig.server)
	{
		std::cout << "Shutting down server...\n";
		networkConfig.server->Shutdown();
		delete networkConfig.server;
	}

	if (networkConfig.client)
	{
		std::cout << "Disconnecting client...\n";
		delete networkConfig.client;
	}
}

void GameConfigManager::InitNetwork()
{
	if (networkConfig.isMultiplayer)
	{
		static bool networkInitialized = false;
		if (!networkInitialized) {
			std::cout << "Initializing network system...\n";
			NetworkBase::Initialise();
			networkInitialized = true;
			std::cout << "Network system initialized\n";
		}
	}
}

void GameConfigManager::CreateServer()
{
	try
	{
		std::cout << "Creating server...\n";
		networkConfig.server = new GameServer(NetworkBase::GetDefaultPort(), 8);
		networkConfig.playerID = 1;  // Server is always player 1
		std::cout << "Server started successfully on port " << NetworkBase::GetDefaultPort() << "\n";
	}
	catch (const std::exception& e)
	{
		std::cout << "Server creation failed: " << e.what() << "\n";
		delete networkConfig.server;
		networkConfig.server = nullptr;
		networkConfig.isMultiplayer = false;
		networkConfig.isServer = false;
		std::cout << "Falling back to single player mode\n";
	}
}

void GameConfigManager::CreateClient()
{
	try {
		std::cout << "Creating client...\n";
		networkConfig.client = new GameClient();
		if (!networkConfig.client->netHandle) {
			throw std::runtime_error("Failed to initialize client network handle");
		}

		// Add delay
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::cout << "Attempting to connect to server at " << networkConfig.ip << ":" << NetworkBase::GetDefaultPort() << "\n";

		bool connected = false;

		// Try to connect multiple times
		for (int attempts = 0; attempts < 5; attempts++) {
			std::cout << "Connection attempt " << (attempts + 1) << "...\n";

			if (networkConfig.client->Connect(127, 0, 0, 1, NetworkBase::GetDefaultPort())) {
				connected = true;
				std::cout << "Connected to server successfully!\n";
				break;
			}
			std::cout << "Connection attempt " << (attempts + 1) << " failed, retrying...\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		if (!connected) {
			throw std::runtime_error("Failed to connect to server after multiple attempts");
		}

		networkConfig.playerID = 0;
		networkConfig.client->RegisterPacketHandler(Player_ID_Assignment, this);
	}
	catch (const std::exception& e) {
		std::cout << "Client connection failed: " << e.what() << "\n";
		delete networkConfig.client;
		networkConfig.client = nullptr;
		networkConfig.isMultiplayer = false;
		std::cout << "Falling back to single player mode\n";
	}
}

void GameConfigManager::ReceivePacket(int type, GamePacket* packet, int source)
{
	if (type == Player_ID_Assignment) {
		PlayerIDPacket* idPacket = (PlayerIDPacket*)packet;
		networkConfig.playerID = idPacket->playerID;
		std::cout << "Received player ID from server: " << networkConfig.playerID << "\n";
	}
}