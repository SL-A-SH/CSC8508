#pragma once

#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "GameTechRenderer.h"

namespace NCL {
    namespace CSC8503 {
        class GameLevelManager {
        public:
            GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer, PhysicsSystem* physics);
            ~GameLevelManager();

            virtual void UpdateGame(float dt);

        private:
            GameWorld* world;
            GameTechRenderer* renderer;
            PhysicsSystem* physics;
        };
    }
}