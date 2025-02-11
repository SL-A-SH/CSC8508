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
	Level* level = new SampleLevel();
	level->Init();
	manager->AddLevel(level);
	manager->SetCurrentLevel(level);

	level->AddFloorToWorld(Vector3(0, 0, 0), Vector3(200, 0, 200), Vector4(0.5, 0.5, 0.5, 1));
	level->_mPlayer = level->AddPlayerToWorld(Vector3(0, 50, 0));

	//if(cameraAttached)
	Vector3 playerPosition = level->_mPlayer->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));
	/*else {
		world->GetMainCamera().SetPosition(Vector3(-150, 7, -150));
	}*/
}



GamePlayState::~GamePlayState() {
	delete manager;
}




PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState)
{
	manager->UpdateGame(dt);

	return PushdownResult::NoChange;
}