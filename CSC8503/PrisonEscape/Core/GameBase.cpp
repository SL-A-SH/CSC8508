#include "GameBase.h"
#include "../CSC8503/PrisonEscape/States/GameState.h"
#include "../CSC8503/PrisonEscape/States/MenuState.h"
#include "AudioManager.h"  // Include the header for AudioManager

using namespace NCL;
using namespace CSC8503;

GameTechRenderer* GameBase::renderer = nullptr;
GameWorld* GameBase::world = nullptr;
GameBase* GameBase::instance = nullptr;

AudioManager audio;  // Create an instance of AudioManager

GameBase::GameBase()
{
    gameConfig = nullptr;
}

GameBase::~GameBase() {
    delete stateMachine;
    delete renderer;
    delete world;
    // Shutdown the audio manager when the game is destroyed
}

void GameBase::InitialiseGame() {
    world = new GameWorld();
    renderer = new GameTechRenderer(*world);
    stateMachine = nullptr;
    stateMachine = new PushdownMachine(new MenuState());

    if (!audio.Initialize()) {
        std::cerr << "Failed to initialize AudioManager!" << std::endl;
        return;
    }

    std::string soundFile = "PrisonEscape/Assets/SFX/Shotgun.wav"; // Replace with actual file path
    audio.LoadSound(soundFile);
    audio.PlaySound(soundFile, false); // Play sound once

    // Let sound play for a few seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop the sound
    audio.StopSound(soundFile);

    return;

}

void GameBase::UpdateGame(float dt) {
    renderer->Update(dt);
    stateMachine->Update(dt);
    renderer->Render();
    Debug::UpdateRenderables(dt);
    bool spamsound = true;
    // Example: Play the sound in the Update loop (or trigger it based on some condition)
    
}

GameBase* GameBase::GetGameBase()
{
    if (instance == nullptr)
    {
        instance = new GameBase();
    }
    return instance;
}