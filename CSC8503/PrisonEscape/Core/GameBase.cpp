#include "GameBase.h"
#include "GameTechRenderer.h"
#include "PrisonEscape/States/GameState.h"
#include "PrisonEscape/States/MenuState.h"
#include "PrisonEscape/Core/ImGuiManager.h"
#include "PrisonEscape/Core/GameConfigManager.h"
#include "AudioManager.h"
#include "GameSettingManager.h"


using namespace NCL;
using namespace CSC8503;

GameTechRenderer* GameBase::renderer = nullptr;
GameWorld* GameBase::world = nullptr;
GameBase* GameBase::instance = nullptr;
GameSettingManager gameSettings;  // Create an instance of GameSettingManager
AudioManager audio(&gameSettings); // Pass it to AudioManager


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
	ImGuiManager::Initialize();


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
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::H))
	{
		renderer->USEDEBUGMODE = !renderer->USEDEBUGMODE;
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


void GameBase::QuitGame() {
	std::cout << "Shutting down game..." << std::endl;

	// Destroy state machine
	if (stateMachine) {
		delete stateMachine;
		stateMachine = nullptr;
	}

	// Destroy the renderer
	if (renderer) {
		delete renderer;
		renderer = nullptr;
	}

	// Destroy the game world
	if (world) {
		delete world;
		world = nullptr;
	}

	// Clean up ImGui
	//ImGuiManager::Shutdown();

	// Clean up any additional managers or resources
	if (gameConfig) {
		delete gameConfig;
		gameConfig = nullptr;
	}

	// Finally, close the window
	//Window::GetWindow()->Close();
	std::exit(0);
	std::cout << "Game shut down successfully." << std::endl;
}
