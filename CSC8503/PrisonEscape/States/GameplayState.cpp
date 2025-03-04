#include "GameplayState.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "PrisonEscape/Levels/LevelT.h"

using namespace NCL;
using namespace CSC8503;

GamePlayState::GamePlayState(bool multiplayer, bool asServer)
{
	manager = new GameLevelManager(GameBase::GetGameBase()->GetWorld(), GameBase::GetGameBase()->GetRenderer());
}

void GamePlayState::OnAwake()
{
	Level* level = new LevelT();
	level->Init();

	if (gameConfig && gameConfig->networkConfig.isMultiplayer)
	{
		if (gameConfig->networkConfig.isServer)
		{
			level->AddPlayerOneToLevel();
			Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
			GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

			gameConfig->networkConfig.server->SetPlayerConnectedCallback(
				[level](int playerID) {
					level->AddPlayerTwoToLevel();
				}
			);

			gameConfig->networkConfig.server->RegisterPacketHandler(Player_ID_Assignment, level);
			gameConfig->networkConfig.server->RegisterPacketHandler(Player_Position, level);
		}
		else if(gameConfig->networkConfig.client)
		{
			level->AddPlayerTwoToLevel();
			Vector3 playerPosition = level->GetPlayerTwo()->GetTransform().GetPosition();
			GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

			level->AddPlayerOneToLevel();
			// Position it somewhere off to the side initially
			if (level->GetPlayerOne() && level->GetPlayerOne()->GetPlayerObject()) {
				level->GetPlayerOne()->GetPlayerObject()->GetTransform().SetPosition(Vector3(10, -100, 10));
			}

			gameConfig->networkConfig.client->RegisterPacketHandler(Player_Position, level);
			gameConfig->networkConfig.client->RegisterPacketHandler(Player_ID_Assignment, level);
		}
	}
	else
	{
		// Single player mode
		level->AddPlayerOneToLevel();
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
	if (gameConfig && gameConfig->networkConfig.isMultiplayer) {
		if (gameConfig->networkConfig.isServer && gameConfig->networkConfig.server) 
		{
			// Server: Send PlayerOne position to clients
			Level* level = manager->GetCurrentLevel();
			if (level && level->GetPlayerOne() && level->GetPlayerOne()->GetPlayerObject())
			{
				Vector3 pos = level->GetPlayerOne()->GetPlayerObject()->GetTransform().GetPosition();
				PlayerPositionPacket posPacket(1, pos.x, pos.y, pos.z);
				gameConfig->networkConfig.server->SendGlobalPacket(posPacket);
			}

			gameConfig->networkConfig.server->UpdateServer();
		}
		else if (gameConfig->networkConfig.client) 
		{
			// Client: Send PlayerTwo position to server
			Level* level = manager->GetCurrentLevel();
			if (level && level->GetPlayerTwo() && level->GetPlayerTwo()->GetPlayerObject())
			{
				Vector3 pos = level->GetPlayerTwo()->GetPlayerObject()->GetTransform().GetPosition();

				PlayerPositionPacket posPacket(gameConfig->networkConfig.playerID, pos.x, pos.y, pos.z);
				gameConfig->networkConfig.client->SendPacket(posPacket);
			}

			gameConfig->networkConfig.client->UpdateClient();
		}
	}

	manager->UpdateGame(dt);

	return PushdownResult::NoChange;
}