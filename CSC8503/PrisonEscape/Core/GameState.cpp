#include "GameState.h"

using namespace NCL;
using namespace CSC8503;

GamePlayState::GamePlayState() 
{
    world = GameBase::GetWorld();
    physics = new PhysicsSystem(*world);
    
}

GamePlayState::~GamePlayState() {
    delete world;
    
    delete physics;
}

PushdownState::PushdownResult MenuState::OnUpdate(float dt, PushdownState** newState) 
{
    // TODO: Main Menu UI
    // Remove Debug Test Heading and and UI to Scenes/Menus 
    Debug::Print("TEST HEADING", Vector2(25, 30), Debug::WHITE);

    // TODO: Remove Keypress and integrate mouse clicks to start game
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) {
        *newState = new GamePlayState();
        return PushdownResult::Push;
    }

    return PushdownResult::NoChange;
}

PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState) 
{
 

    return PushdownResult::NoChange;
}

PushdownState::PushdownResult PauseState::OnUpdate(float dt, PushdownState** newState) 
{
    // TODO: Pause Menu UI

    return PushdownResult::NoChange;
}

PushdownState::PushdownResult GameOverState::OnUpdate(float dt, PushdownState** newState) 
{
    // TODO: GameOver UI

    return PushdownResult::NoChange;
}