#include "GameplayState.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "PrisonEscape/Levels/LevelT.h"

using namespace NCL;
using namespace CSC8503;

GamePlayState::GamePlayState(bool multiplayer, bool asServer)
{
	gameConfig = new GameConfigManager();

	gameConfig->networkConfig.isMultiplayer = multiplayer;
	gameConfig->networkConfig.isServer = asServer;

	GameBase::GetGameBase()->SetGameConfig(gameConfig);

	if (gameConfig->networkConfig.isMultiplayer)
	{
		gameConfig->InitNetwork();

		if (gameConfig->networkConfig.isServer)
		{
			gameConfig->CreateServer();
		}
		else
		{
			gameConfig->CreateClient();
		}
	}
	
	manager = new GameLevelManager(GameBase::GetGameBase()->GetWorld(), GameBase::GetGameBase()->GetRenderer());
}

void GamePlayState::OnAwake()
{
	Level* level = new LevelT();
	level->Init();

	if (gameConfig->networkConfig.isMultiplayer)
	{
		if (gameConfig->networkConfig.isServer)
		{
			//messy do not keep
			Transform playerOneTransform;
			Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
			playerOne->InitializeController();

			level->AddPlayerToLevel(playerOne);
			Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
			GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

			gameConfig->networkConfig.server->SetPlayerConnectedCallback(
				[this, level](int playerID) {
					Transform playerTwoTransform;
					Player* playerTwo = this->manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");
					playerTwo->InitializeController();
					level->AddPlayerToLevel(playerTwo);
				}
			);

			gameConfig->networkConfig.server->RegisterPacketHandler(Player_ID_Assignment, level);
			gameConfig->networkConfig.server->RegisterPacketHandler(Player_Position, level);
		}
		else if(gameConfig->networkConfig.client)
		{
			Transform playerOneTransform;
			Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
			playerOne->InitializeController();

			Transform playerTwoTransform;
			Player* playerTwo = manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");
			playerTwo->InitializeController();

			level->AddPlayerToLevel(playerTwo);
			Vector3 playerPosition = level->GetPlayerTwo()->GetTransform().GetPosition();
			GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

			level->AddPlayerToLevel(playerOne);
			//Position it somewhere off to the side initially
			if (level->GetPlayerOne() && level->GetPlayerOne()->GetPlayerObject()) {
				level->GetPlayerOne()->GetTransform().SetPosition(Vector3(10, -100, 10));
			}

			gameConfig->networkConfig.client->RegisterPacketHandler(Player_Position, level);
			gameConfig->networkConfig.client->RegisterPacketHandler(Player_ID_Assignment, level);
		}
	}
	else
	{
		// Single player mode
		Transform playerTransform;
		Player* player = manager->AddPlayerToWorld(playerTransform, "playerOne");
		player->InitializeController();

		level->AddPlayerToLevel(player);
		Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
		GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));
	}

	manager->AddLevel(level);
	manager->SetCurrentLevel(level);
}

GamePlayState::~GamePlayState() 
{
	delete manager;
	delete gameConfig;
}

PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState) 
{
	if (gameConfig->networkConfig.isMultiplayer) {
		if (gameConfig->networkConfig.isServer) 
		{
			// Server: Send PlayerOne position to clients
			Level* level = manager->GetCurrentLevel();
			if (level && level->GetPlayerOne())
			{
				Vector3 pos = level->GetPlayerOne()->GetTransform().GetPosition();
				PlayerPositionPacket posPacket(1, pos.x, pos.y, pos.z);
				gameConfig->networkConfig.server->SendGlobalPacket(posPacket);
			}

			gameConfig->networkConfig.server->UpdateServer();
		}
		else if (gameConfig->networkConfig.client) 
		{
			// Client: Send PlayerTwo position to server
			Level* level = manager->GetCurrentLevel();
			if (level && level->GetPlayerTwo())
			{
				Vector3 pos = level->GetPlayerTwo()->GetTransform().GetPosition();
				PlayerPositionPacket posPacket(gameConfig->networkConfig.playerID, pos.x, pos.y, pos.z);
				gameConfig->networkConfig.client->SendPacket(posPacket);
			}

			gameConfig->networkConfig.client->UpdateClient();
		}
	}

	manager->UpdateGame(dt);

	return PushdownResult::NoChange;
}