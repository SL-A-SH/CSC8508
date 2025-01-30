#include "PrisonGameManager.h"

using namespace NCL;
using namespace CSC8503;

PrisonGameManager::PrisonGameManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer, PhysicsSystem* existingPhysics)
{
    world = existingWorld;
    renderer = existingRenderer;
    physics = existingPhysics;

    // TODO: Initialize assets and level creation
}

PrisonGameManager::~PrisonGameManager()
{
    delete world;
    delete renderer;
    delete physics;
}

void PrisonGameManager::UpdateGame(float dt)
{
    // TODO: Level Updates
    // TODO: Remove Debug
    Debug::Print("LEVELS", Vector2(25, 30), Debug::WHITE);
}
