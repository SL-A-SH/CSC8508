#include "GameplayState.h"
#include "PrisonEscape/Levels/SampleLevel.h"

using namespace NCL;
using namespace CSC8503;

GamePlayState::GamePlayState()
{
	manager = new GameLevelManager(GameBase::GetGameBase()->GetWorld(), GameBase::GetGameBase()->GetRenderer());
}

void GamePlayState::OnAwake()
{
	SampleLevel* level = new SampleLevel();
	level->Init();
	Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

	manager->AddLevel(level);
	manager->SetCurrentLevel(level);
}

GamePlayState::~GamePlayState() {
	delete manager;
}

PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState)
{
	manager->UpdateGame(dt);

	return PushdownResult::NoChange;
}