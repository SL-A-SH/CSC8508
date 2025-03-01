#include "GameLevelManager.h"
#include "PrisonEscape/Levels/SampleLevel.h"
using namespace NCL;
using namespace CSC8503;

GameLevelManager::GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer)
{
	world = existingWorld;
	renderer = existingRenderer;
	physics = new PhysicsSystem(*world);
	physics->UseGravity(true);
	// Move to another place if needed



}

GameLevelManager::~GameLevelManager()
{

	delete physics;
}

void GameLevelManager::UpdateGame(float dt)
{
	// TODO: Level Updates
	// TODO: Remove Debug
	physics->Update(dt);

	GetCurrentLevel()->Update(dt);
	Debug::Print("LEVELS", Vector2(25, 30), Debug::WHITE);
}
