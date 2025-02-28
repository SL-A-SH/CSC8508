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
	Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

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
	if (gameConfig->networkConfig.isMultiplayer)
	{
		if (gameConfig->networkConfig.isServer && gameConfig->networkConfig.server) 
		{
			gameConfig->networkConfig.server->UpdateServer();
		}
		else if (gameConfig->networkConfig.client)
		{
			gameConfig->networkConfig.client->UpdateClient();
		}
	}

	manager->UpdateGame(dt);

	return PushdownResult::NoChange;
}