#include "GameBase.h"
#include "../CSC8503/PrisonEscape/States/GameState.h"
#include "../CSC8503/PrisonEscape/States/MenuState.h"
#include "AudioManager.h"  // Include the header for AudioManager

using namespace NCL;
using namespace CSC8503;

GameTechRenderer* GameBase::renderer = nullptr;
GameWorld* GameBase::world = nullptr;
GameBase* GameBase::instance = nullptr;

AudioManager audioManager;  // Create an instance of AudioManager

GameBase::GameBase()
{
    gameConfig = nullptr;
}

GameBase::~GameBase() {
    delete stateMachine;
    delete renderer;
    delete world;
    audioManager.Shutdown();  // Shutdown the audio manager when the game is destroyed
}

void GameBase::InitialiseGame() {
    world = new GameWorld();
    renderer = new GameTechRenderer(*world);
    stateMachine = nullptr;
    stateMachine = new PushdownMachine(new MenuState());

    // Initialize the audio manager and load a sound
    if (audioManager.Initialize()) {
        if (audioManager.LoadSound("PrisonEscape/Assets/SFX/Shotgun.wav", "explosion")) {
            std::cout << "Sound loaded successfully!\n";
        }
        else {
            std::cerr << "Failed to load sound!\n";
        }
    }

}

void GameBase::UpdateGame(float dt) {
    renderer->Update(dt);
    stateMachine->Update(dt);
    renderer->Render();
    Debug::UpdateRenderables(dt);
    bool spamsound = true;
    // Example: Play the sound in the Update loop (or trigger it based on some condition)
    if (spamsound) {  // Replace with an actual condition to play the sound
        audioManager.PlaySound("explosion");
    }
}

GameBase* GameBase::GetGameBase()
{
    if (instance == nullptr)
    {
        instance = new GameBase();
    }
    return instance;
}