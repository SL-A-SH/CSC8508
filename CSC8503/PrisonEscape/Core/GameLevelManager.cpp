#include "GameLevelManager.h"

using namespace NCL;
using namespace CSC8503;

GameLevelManager::GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer, PhysicsSystem* existingPhysics)
{
    world = existingWorld;
    renderer = existingRenderer;
    physics = existingPhysics;

   
}

GameLevelManager::~GameLevelManager()
{
    delete world;
    delete renderer;
    delete physics;
}

void GameLevelManager::UpdateGame(float dt)
{
    // TODO: Level Updates
    // TODO: Remove Debug
    Debug::Print("LEVELS", Vector2(25, 30), Debug::WHITE);
}
