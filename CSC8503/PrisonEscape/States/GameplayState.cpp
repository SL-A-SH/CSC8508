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
	manager->AddLevel(level);
	manager->SetCurrentLevel(level);
	level->AddSphereToWorld(Vector3(0, 10, 0), 1.0f, 1.0f);
}



GamePlayState::~GamePlayState() {
	delete manager;
}




PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState)
{
	manager->GetCurrentLevel()->Update(dt);

	return PushdownResult::NoChange;
}