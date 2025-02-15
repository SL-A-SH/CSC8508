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
		class MenuState : public GameState {

		public:
			MenuState() {}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override;
			void OnAwake() override;

			void DrawMainMenuPanel();
			void DrawSettingPanel();
			void DrawAudioSettingPanel();
			void DrawVideoSettingPanel();

			ImFont* menuHeading = nullptr;
			ImFont* buttonFont = nullptr;
			ImFont* headerFont = nullptr;

			int volume;

		private:
			std::function<void(PushdownState**)> stateChangeAction = nullptr;
		};
	}
}