#pragma once
#include "PushdownState.h"
#include "GameWorld.h"
#include "Window.h"
#include "PhysicsSystem.h"
#include "GameState.h"

#include "../CSC8503/PrisonEscape/Core/GameConfigManager.h"
#include "../CSC8503/PrisonEscape/Core/GameLevelManager.h"

namespace NCL {
	namespace CSC8503 {
		class GamePlayState : public GameState {
		public:
			GamePlayState(bool multiplayer, bool asServer, GameConfigManager* config);
			~GamePlayState();

			void OnAwake() override;
			PushdownResult OnUpdate(float dt, PushdownState** newState) override;
			void SetGameConfig(GameConfigManager* config) {
				if (gameConfig != nullptr && gameConfig != config) {
					delete gameConfig;
				}
				gameConfig = config;
				GameBase::GetGameBase()->SetGameConfig(gameConfig);
			}

			void DrawHUDPanel();

		private:
			GameLevelManager* manager;
			GameConfigManager* gameConfig;

			Texture* heartFilledTexture = nullptr;
			Texture* heartEmptyTexture = nullptr;

		public:
			GameConfigManager* GetGameConfig() const { return gameConfig; }
		};
	}
}