#pragma once
#pragma once

#include "PushdownState.h"
#include "GameWorld.h"
#include "Window.h"
#include "Debug.h"
#include "PhysicsSystem.h"
#include "../CSC8503/PrisonEscape/Core/GameBase.h"
#include "../CSC8503/PrisonEscape/Core/GameLevelManager.h"
#include "imgui/imgui.h"

namespace NCL {
	namespace CSC8503 {
		class GameState : public PushdownState {
		public:
			GameState() {}
			virtual ~GameState() {}
		};

		class MenuState;
		class GamePlayState;
		class PauseState;
		class GameOverState;




		class PauseState : public GameState {
		public:
			PauseState() {}
			PushdownResult OnUpdate(float dt, PushdownState** newState) override;
		};

		class GameOverState : public GameState {
		public:
			GameOverState() {
			}
			PushdownResult OnUpdate(float dt, PushdownState** newState) override;
		};
	}
}