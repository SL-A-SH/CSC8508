#include "GameBase.h"
#include "PrisonEscape/States/GameState.h"
#include "PrisonEscape/States/MenuState.h"

using namespace NCL;
using namespace CSC8503;

GameTechRenderer* GameBase::renderer = nullptr;
GameWorld* GameBase::world = nullptr;
GameBase* GameBase::instance = nullptr;

GameBase::GameBase()
{
	gameConfig = nullptr;
}

GameBase::~GameBase() {
	delete stateMachine;
	delete renderer;
	delete world;
}

void GameBase::InitialiseGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	stateMachine = nullptr;
	stateMachine = new PushdownMachine(new MenuState());
}

void GameBase::UpdateGame(float dt) {
	stateMachine->Update(dt);
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

GameBase* GameBase::GetGameBase()
{
	if (instance == nullptr)
	{
		instance = new GameBase();
	}
	return instance;
}
