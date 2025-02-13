#pragma once

#include "GameTechRenderer.h"
#include "PushdownMachine.h"
#include "GameWorld.h"


namespace NCL {
	namespace CSC8503 {
		class GameBase {

		public:
			GameBase();
			~GameBase();

			virtual void UpdateGame(float dt);
			void InitialiseGame();

		protected:
			static GameTechRenderer* renderer;
			static GameWorld* world;
			static GameBase* instance;

			PushdownMachine* stateMachine;

		public:
			static GameTechRenderer* GetRenderer() { return renderer; }
			static GameWorld* GetWorld() { return world; }
			static GameBase* GetGameBase();
			static PushdownMachine* GetStateMachine() { return instance->stateMachine; }
		};
	}
}