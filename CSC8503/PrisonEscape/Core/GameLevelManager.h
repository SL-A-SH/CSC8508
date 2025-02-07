#pragma once

#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "GameTechRenderer.h"
#include "../CSC8503/PrisonEscape/Levels/Level.h"

namespace NCL {
	namespace CSC8503 {
		class GameLevelManager {
		public:
			GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer);
			~GameLevelManager();
			virtual void UpdateGame(float dt);



		public:
			Level* GetCurrentLevel() { return currentLevel; }
			void SetCurrentLevel(Level* level) { currentLevel = level; }
			void AddLevel(Level* newLevel) { levelStack.push(newLevel); }
		private:
			GameWorld* world;
			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			Level* currentLevel;
			std::stack<Level*> levelStack;


		};
	}
}