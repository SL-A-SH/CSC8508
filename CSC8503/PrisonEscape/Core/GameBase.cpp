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
    if (audio.Initialize()) {
        audio.PrintOutputDevices();  // Print out available audio output devices and the current one
        audio.SelectOutputDevice(0); // Select the first output device
    }
    else {
        std::cerr << "Failed to initialize AudioManager!" << std::endl;
    }

    std::string soundFile = "PrisonEscape/Assets/SFX/Shotgun.wav";
    audio.LoadSound(soundFile);
    audio.PlaySound(soundFile);  // Play without loop for testing
}

void GameBase::UpdateGame(float dt) {
    renderer->Update(dt);
    stateMachine->Update(dt);
    renderer->Render();
    Debug::UpdateRenderables(dt);
    audio.Update();
    
}

GameBase* GameBase::GetGameBase()
{
    if (instance == nullptr)
    {
        instance = new GameBase();
    }
    return instance;
}