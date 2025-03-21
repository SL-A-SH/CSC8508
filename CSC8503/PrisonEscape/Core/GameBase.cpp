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
GameSettingManager* GameBase::gameSettings = nullptr;
AudioManager* GameBase::audioManager = nullptr;
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
	gameSettings = new GameSettingManager();
	audioManager = new AudioManager();
	ImGuiManager::Initialize();


	if (!audioManager->Initialize()) {
		std::cerr << "Failed to initialize AudioManager!" << std::endl;
		return;
	}
	if (audioManager->Initialize()) {
		audioManager->PrintOutputDevices();  // Print out available audio output devices and the current one
		audioManager->SelectOutputDevice(0); // Select the first output device
	}
	else {
		std::cerr << "Failed to initialize AudioManager!" << std::endl;
	}

	std::string soundFile = "PrisonEscape/Assets/SFX/Shotgun.wav";
	audioManager->LoadSound(soundFile);
	audioManager->PlaySound(soundFile);  // Play without loop for testing
}

void GameBase::UpdateGame(float dt) {
	renderer->Update(dt);
	stateMachine->Update(dt);
	renderer->Render();
	Debug::UpdateRenderables(dt);
	audioManager->Update();
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

	if (stateMachine) {
		delete stateMachine;
		stateMachine = nullptr;
	}

	if (renderer) {
		delete renderer;
		renderer = nullptr;
	}

	if (world) {
		delete world;
		world = nullptr;
	}

	if (gameConfig) {
		delete gameConfig;
		gameConfig = nullptr;
	}

	std::exit(0);
	std::cout << "Game shut down successfully." << std::endl;
}
