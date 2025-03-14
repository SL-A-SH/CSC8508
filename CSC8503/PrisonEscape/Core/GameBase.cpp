#include "GameBase.h"
#include "PrisonEscape/States/GameState.h"
#include "PrisonEscape/States/MenuState.h"
#include "PrisonEscape/Core/ImGuiManager.h"

using namespace NCL;
using namespace CSC8503;

GameTechRenderer* GameBase::renderer = nullptr;
GameWorld* GameBase::world = nullptr;
GameBase* GameBase::instance = nullptr;

GameBase::GameBase()
{
	gameConfig = nullptr;
}

GameBase::~GameBase() {
	delete stateMachine;
	delete renderer;
	delete world;
}

void GameBase::InitialiseGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	stateMachine = nullptr;
	stateMachine = new PushdownMachine(new MenuState());
	ImGuiManager::Initialize();
}

void GameBase::UpdateGame(float dt) {
	stateMachine->Update(dt);
	renderer->Render();
	Debug::UpdateRenderables(dt);
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
