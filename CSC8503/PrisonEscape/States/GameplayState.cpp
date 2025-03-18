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

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::P) && !gameConfig->networkConfig.isMultiplayer)
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

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::TAB) && gameConfig->networkConfig.isMultiplayer)
	{
		friendsPanelVisible = !friendsPanelVisible;
		if (friendsPanelVisible)
		{
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("FriendsPanel", [this]() { DrawFriendsPanel(); });
		}
		else
		{
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("FriendsPanel");
		}
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

void GamePlayState::DrawFriendsPanel()
{
	// Get the Steam manager instance
	SteamManager* steamManager = SteamManager::GetInstance();

	if (!steamManager || !steamManager->IsInitialized()) {
		// Steam not available - show error message
		ImGuiManager::DrawMessagePanel("Steam Error", "Steam is not initialized. Please restart the game and make sure Steam is running.", ImVec4(1, 0, 0, 1));
		return;
	}

	// Get the list of friends from Steam
	std::vector<std::pair<std::string, uint64_t>> allFriends = steamManager->GetFriendsList();

	// Filter online friends only
	std::vector<std::pair<std::string, uint64_t>> onlineFriends;
	for (const auto& friendInfo : allFriends) {
		if (steamManager->IsFriendOnline(friendInfo.second) && steamManager->DoesFriendOwnGame(friendInfo.second)) {
			onlineFriends.push_back(friendInfo);
		}
	}


	DrawFriendsListWindow(onlineFriends);
}

void GamePlayState::DrawFriendsListWindow(const std::vector<std::pair<std::string, uint64_t>>& onlineFriends)
{
	// Get the screen size
	ImVec2 screenSize = ImGui::GetIO().DisplaySize;

	// Set the desired window size
	ImVec2 windowSize(600, 400); // You can adjust the size as per your needs

	// Calculate the position to center the window
	ImVec2 windowPos = ImVec2((screenSize.x - windowSize.x) * 0.5f, (screenSize.y - windowSize.y) * 0.5f);

	// Set the next window position and size before calling Begin()
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

	if (ImGui::Begin("Friends List", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {

		// Custom drawing for the friends list
		ImVec2 windowSize = ImGui::GetWindowSize();
		float startY = windowSize.y * 0.25f; // Start below the header

		SteamManager* steamManager = SteamManager::GetInstance();
		if (gameConfig && gameConfig->networkConfig.isUsingSteam) {
			uint64_t lobbyID = steamManager->GetCurrentLobbyID();
			if (lobbyID != 0) {
				ImGui::SetCursorPos(ImVec2(windowSize.x * 0.1f, startY));
				ImGui::TextColored(ImVec4(0, 1, 1, 1), "Your Lobby ID: %llu", lobbyID);
				startY += 50; // Move down a bit for the rest of the content
			}
		}

		if (onlineFriends.empty()) {
			// No online friends found
			ImGui::SetCursorPos(ImVec2(windowSize.x * 0.2f, startY));
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "No online friends found who own the game");
		}
		else {
			// Draw each friend with an invite button
			float itemHeight = windowSize.y * 0.08f;
			float padding = windowSize.y * 0.02f;
			float currentY = startY;

			ImGui::PushFont(ImGuiManager::GetButtonFont());

			for (const auto& friendInfo : onlineFriends) {
				const std::string& friendName = friendInfo.first;
				uint64_t friendID = friendInfo.second;

				// Friend name
				ImGui::SetCursorPos(ImVec2(windowSize.x * 0.1f, currentY));
				ImGui::Text("%s", friendName.c_str());

				// Status indicator (green circle for online)
				ImGui::SetCursorPos(ImVec2(windowSize.x * 0.5f, currentY + itemHeight * 0.5f));
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "?"); // Green dot for online

				// Invite button
				ImGui::SetCursorPos(ImVec2(windowSize.x * 0.6f, currentY));
				std::string buttonLabel = "Invite##" + std::to_string(friendID);

				if (ImGui::Button(buttonLabel.c_str(), ImVec2(windowSize.x * 0.25f, itemHeight))) {
					// Send invite to this friend
					steamManager->SendGameInvite(friendID);

					// Show confirmation message
					GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("InviteSentPanel", [friendName, this]() {
						ImGuiManager::DrawMessagePanel("Invite Sent",
							"Game invite sent to " + friendName,
							ImVec4(0, 1, 0, 1),
							[this]() {
								GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("InviteSentPanel");
								GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("FriendsPanel", [this]() { DrawFriendsPanel(); });
							});
						});

					GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("FriendsPanel");
				}

				currentY += itemHeight + padding;
			}

			ImGui::PopFont();
		}

		ImGui::End(); // End the ImGui window
	}
}
