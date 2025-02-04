#pragma once
#pragma once

#include "PushdownState.h"
#include "GameWorld.h"
#include "Window.h"
#include "Debug.h"
#include "PhysicsSystem.h"
#include "GameBase.h"


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

        class MenuState : public GameState {
        public:
            MenuState() {}
            PushdownResult OnUpdate(float dt, PushdownState** newState) override;
        };

        class GamePlayState : public GameState {
        public:
            GamePlayState();
            ~GamePlayState();
            PushdownResult OnUpdate(float dt, PushdownState** newState) override;

        private:
            GameWorld* world;
            PhysicsSystem* physics;
          
        };

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