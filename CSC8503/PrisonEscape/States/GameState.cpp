#include "GameState.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "../CSC8503/PrisonEscape/Core/GameLevelManager.h"

using namespace NCL;
using namespace CSC8503;


GamePlayState::GamePlayState()
{

	manager = new GameLevelManager(GameBase::GetGameBase()->GetWorld(), GameBase::GetGameBase()->GetRenderer());

}

GamePlayState::~GamePlayState() {
	delete world;

	delete physics;
}




PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState)
{
	manager->GetCurrentLevel()->Update(dt);

	return PushdownResult::NoChange;
}

PushdownState::PushdownResult PauseState::OnUpdate(float dt, PushdownState** newState)
{
	// TODO: Pause Menu UI

	return PushdownResult::NoChange;
}

PushdownState::PushdownResult GameOverState::OnUpdate(float dt, PushdownState** newState)
{
	// TODO: GameOver UI

	return PushdownResult::NoChange;
}