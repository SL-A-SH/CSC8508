#pragma once

#include "../CSC8503/PrisonEscape/Core/GameLevelManager.h"
#include "PushdownState.h"
#include "GameWorld.h"
#include "Window.h"
#include "Debug.h"
#include "PhysicsSystem.h"
#include "GameState.h"
#include "imgui/imgui.h"

namespace NCL {
	namespace CSC8503 {
		class SteamManager;

		class MenuState : public GameState {
		public:
			MenuState();
			~MenuState();

			PushdownResult OnUpdate(float dt, PushdownState** newState) override;
			void OnAwake() override;

			void DrawMainMenuPanel();
			void DrawSettingPanel();
			void DrawAudioSettingPanel();
			void DrawVideoSettingPanel();
			void DrawMultiplayerPanel();
			void DrawSteamLobbyPanel();

			// Connection handling methods
			void StartServerProcess();
			void StartClientProcess();
			void DrawConnectionMessagePanel();

			void InitializeSteam();
			void HandleSteamInvite(uint64_t friendSteamID);
			void JoinSteamLobby(uint64_t lobbyID);

		private:
			enum class ConnectionStage {
				None,
				Creating,
				Connecting,
				Success,
				Failed
			};

			std::function<void(PushdownState**)> stateChangeAction = nullptr;
			int volume = 50;
			int brightness = 50;

			// Network connection state
			bool isConnecting = false;
			ConnectionStage connectionStage;
			bool networkAsServer;
			float connectionTimer;
			int connectionAttempt;
			GameConfigManager* gameConfig;

			SteamManager* steamManager;
			bool useSteamNetworking = false;
		};
	}
}