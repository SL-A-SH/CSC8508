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
		class Level;

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
			void DrawFriendsPanel();
			void DrawFriendsListWindow(const std::vector<std::pair<std::string, uint64_t>>& onlineFriends);

			bool FriendsPanelVisible() const { return friendsPanelVisible; }

		private:
			GameLevelManager* manager;
			GameConfigManager* gameConfig;
			bool friendsPanelVisible = false;

			void InitializeSteamMultiplayer(Level* level);
			bool InitializeSteamNetworking();
			void InitializeMultiplayer(Level* level);
			void InitializeSinglePlayer(Level* level);

			void SetupServer(Level* level);
			void SetupClient(Level* level);
			void SetupClientPlayer(Level* level);

			void RegisterServerPacketHandlers();
			void RegisterClientPacketHandlers();



			Texture* heartFilledTexture = nullptr;
			Texture* heartEmptyTexture = nullptr;

		public:
			GameConfigManager* GetGameConfig() const { return gameConfig; }
		};
	}
}