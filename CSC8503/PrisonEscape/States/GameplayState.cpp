#include "GameplayState.h"
#include "GameoverState.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "PrisonEscape/Levels/LevelOne.h"
#include "PrisonEscape/Levels/LevelT.h"
#include "PauseState.h"
#include "PrisonEscape/Core/Networking/SteamManager.h"

using namespace NCL;
using namespace CSC8503;

GamePlayState::GamePlayState(bool multiplayer, bool asServer, GameConfigManager* config)
{
	this->gameConfig = config;
	manager = new GameLevelManager(GameBase::GetGameBase()->GetWorld(), GameBase::GetGameBase()->GetRenderer());
	Level* level = new LevelT();
	level->Init();

	bool useSteamNetworking = InitializeSteamNetworking();

	if (gameConfig->networkConfig.isMultiplayer) {
		if (useSteamNetworking) {
			InitializeSteamMultiplayer(level);
		}
		else {
			InitializeMultiplayer(level);
		}
	}
	else {
		InitializeSinglePlayer(level);
	}

	manager->AddLevel(level);
	manager->SetCurrentLevel(level);
}

void GamePlayState::OnAwake()
{
	GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("ConnectionPanel");

	GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("HUDPanel", [this]() { DrawHUDPanel(); });
}

GamePlayState::~GamePlayState()
{
	delete manager;
	delete gameConfig;
}

PushdownState::PushdownResult GamePlayState::OnUpdate(float dt, PushdownState** newState)
{
	SteamManager* steamManager = SteamManager::GetInstance();
	bool useSteamNetworking = steamManager && steamManager->IsInitialized();

	if (gameConfig->networkConfig.isMultiplayer) {

		if (useSteamNetworking)
		{
			Level* level = manager->GetCurrentLevel();
			if (level)
			{
				Player* playerToSync = steamManager->IsLobbyOwner() ? level->GetPlayerOne() : level->GetPlayerTwo();
				if (playerToSync) {
					Vector3 pos = playerToSync->GetTransform().GetPosition();
					steamManager->SendPlayerPosition(pos);
				}
			}
			steamManager->Update();
		}
		else
		{
			if (gameConfig->networkConfig.isServer)
			{
				// Server: Send PlayerOne position to clients
				Level* level = manager->GetCurrentLevel();
				if (level && level->GetPlayerOne())
				{
					Vector3 pos = level->GetPlayerOne()->GetTransform().GetPosition();
					PlayerPositionPacket posPacket(1, pos.x, pos.y, pos.z);
					gameConfig->networkConfig.server->SendGlobalPacket(posPacket);
				}

				gameConfig->networkConfig.server->UpdateServer();
			}
			else if (gameConfig->networkConfig.client)
			{
				// Client: Send PlayerTwo position to server
				Level* level = manager->GetCurrentLevel();
				if (level && level->GetPlayerTwo())
				{
					Vector3 pos = level->GetPlayerTwo()->GetTransform().GetPosition();
					PlayerPositionPacket posPacket(gameConfig->networkConfig.playerID, pos.x, pos.y, pos.z);
					gameConfig->networkConfig.client->SendPacket(posPacket);
				}

				gameConfig->networkConfig.client->UpdateClient();
			}
		}
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::P))
	{
		*newState = new PauseState(gameConfig);
		return PushdownResult::Push;
	}
	manager->UpdateGame(dt);

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1))
	{
		*newState = new GameOverState(GameOverReason::OutOfLives);
		return PushdownResult::Push;
	}

	return PushdownResult::NoChange;
}

void GamePlayState::InitializeSteamMultiplayer(Level* level)
{
	SteamManager* steamManager = SteamManager::GetInstance();
	bool isHost = steamManager->IsLobbyOwner();

	// Create both players regardless of host status
	Transform playerOneTransform;
	Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
	playerOne->InitializeController();

	Transform playerTwoTransform;
	Player* playerTwo = manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");
	playerTwo->InitializeController();

	// Determine which player this client controls based on Steam role
	if (isHost)
	{
		level->AddPlayerToLevel(playerOne);
		level->AddPlayerToLevel(playerTwo);
		// Position playerTwo off-screen initially until connected
		playerTwo->GetTransform().SetPosition(Vector3(10, -100, 10));
	}
	else
	{
		level->AddPlayerToLevel(playerTwo);
		level->AddPlayerToLevel(playerOne);
		// Position playerOne off-screen initially
		playerOne->GetTransform().SetPosition(Vector3(10, -100, 10));
	}

	// Set up camera for the active player
	Player* activePlayer = isHost ? playerOne : playerTwo;
	Vector3 playerPosition = activePlayer->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

	// Register with Steam for position updates
	steamManager->SetPlayerUpdateCallback([this, steamManager, playerOne, playerTwo](uint64_t steamID, const Vector3& pos) {
		// Update the appropriate player based on Steam ID
		if (steamManager->IsLobbyOwner())
		{
			// Host updates playerTwo
			playerTwo->GetTransform().SetPosition(pos);
		}
		else
		{
			// Client updates playerOne
			playerOne->GetTransform().SetPosition(pos);
		}
		});
}

void GamePlayState::DrawHUDPanel() {

	ImGui::Begin("HUD Panel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	// Display Timer
	ImGui::Text("Time: %.2f s", 25.0f);

	// Display Health Bar
	ImGui::Text("Health:");
	ImGui::ProgressBar(50 / 100.0f, ImVec2(200, 20));


	ImGui::End();
}

bool GamePlayState::InitializeSteamNetworking() {
	SteamManager* steamManager = SteamManager::GetInstance();
	bool useSteamNetworking = false;
	if (steamManager && steamManager->IsInitialized()) {
		useSteamNetworking = true;
		std::cout << "Using Steam for networking" << std::endl;
	}
	return useSteamNetworking;
}

void GamePlayState::InitializeMultiplayer(Level* level) {
	if (gameConfig->networkConfig.isServer && gameConfig->networkConfig.server) {
		SetupServer(level);
	}
	else if (gameConfig->networkConfig.client) {
		SetupClient(level);
	}
}

void GamePlayState::InitializeSinglePlayer(Level* level) {
	std::cout << "Single player mode..." << std::endl;

	// Create and initialize player one (single-player)
	Transform playerTransform;
	Player* player = manager->AddPlayerToWorld(playerTransform, "playerOne");
	player->InitializeController();
	level->AddPlayerToLevel(player);

	// Set the camera position for the single-player
	Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));
}

void GamePlayState::SetupServer(Level* level) {
	std::cout << "Setting up server..." << std::endl;

	// Create and initialize player one (server)
	Transform playerOneTransform;
	Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
	playerOne->InitializeController();
	level->AddPlayerToLevel(playerOne);

	// Set the camera position for the server player
	Vector3 playerPosition = level->GetPlayerOne()->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

	// Set up server callbacks for client connections
	gameConfig->networkConfig.server->SetPlayerConnectedCallback([this, level](int playerID) {
		SetupClientPlayer(level);
		});

	// Register packet handlers for server
	RegisterServerPacketHandlers();
}

void GamePlayState::SetupClient(Level* level) {
	std::cout << "Setting up client..." << std::endl;

	// Create and initialize both players (client-side)
	Transform playerOneTransform;
	Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
	playerOne->InitializeController();

	Transform playerTwoTransform;
	Player* playerTwo = manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");
	playerTwo->InitializeController();

	level->AddPlayerToLevel(playerOne);
	level->AddPlayerToLevel(playerTwo);

	// Set the initial camera position for the client player
	Vector3 playerPosition = level->GetPlayerTwo()->GetTransform().GetPosition();
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));

	// Register packet handlers for the client
	RegisterClientPacketHandlers();
}

void GamePlayState::SetupClientPlayer(Level* level) {
	Transform playerTwoTransform;
	Player* playerTwo = this->manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");
	playerTwo->InitializeController();
	level->AddPlayerToLevel(playerTwo);
}

void GamePlayState::RegisterServerPacketHandlers() {
	gameConfig->networkConfig.server->RegisterPacketHandler(Player_ID_Assignment, manager->GetCurrentLevel());
	gameConfig->networkConfig.server->RegisterPacketHandler(Player_Position, manager->GetCurrentLevel());
}

void GamePlayState::RegisterClientPacketHandlers() {
	gameConfig->networkConfig.client->RegisterPacketHandler(Player_Position, manager->GetCurrentLevel());
	gameConfig->networkConfig.client->RegisterPacketHandler(Player_ID_Assignment, manager->GetCurrentLevel());
}