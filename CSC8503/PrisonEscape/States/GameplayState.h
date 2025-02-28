#pragma once
#include "PushdownState.h"
#include "GameWorld.h"
#include "Window.h"
#include "PhysicsSystem.h"
#include "GameState.h"

#include "../CSC8503/PrisonEscape/Core/GameConfigManager.h"
#include "../CSC8503/PrisonEscape/Core/GameLevelManager.h"
#include "../CSC8503/PrisonEscape/Prefabs/Player/PlayerOne.h" 

namespace NCL {
	namespace CSC8503 {
		class GamePlayState : public GameState {
		public:
			GamePlayState(bool multiplayer, bool asServer);
			~GamePlayState();

			void OnAwake() override;
			PushdownResult OnUpdate(float dt, PushdownState** newState) override;

		private:
			GameLevelManager* manager;
			GameConfigManager* gameConfig;
		};
	}
}