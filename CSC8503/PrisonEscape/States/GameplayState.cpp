#include "GameplayState.h"
#include "GameoverState.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "PrisonEscape/Levels/LevelOne.h"
#include "PrisonEscape/Levels/LevelT.h"
#include "PauseState.h"

using namespace NCL;
using namespace CSC8503;

GamePlayState::GamePlayState(bool multiplayer, bool asServer, GameConfigManager* config)
{
	manager = new GameLevelManager(GameBase::GetGameBase()->GetWorld(), GameBase::GetGameBase()->GetRenderer());
	Level* level = new LevelT();
	level->Init();
	this->gameConfig = config;
	if (gameConfig->networkConfig.isMultiplayer)
	{
		InitializeMultiplayer(level, asServer);
	}
	else
	{
		InitializeSinglePlayer(level);
	}

	manager->AddLevel(level);
	manager->SetCurrentLevel(level);

	LoadTextures();


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
	if (gameConfig->networkConfig.isMultiplayer) {
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

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::P))
	{
		*newState = new PauseState();
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

void GamePlayState::InitializeSinglePlayer(Level* level)
{
	Transform playerTransform;
	Player* player = manager->AddPlayerToWorld(playerTransform, "playerOne");
	player->InitializeController();

	level->AddPlayerToLevel(player);
	SetCameraToPlayer(level->GetPlayerOne());
}

void GamePlayState::InitializeMultiplayer(Level* level, bool asServer)
{
	if (asServer)
	{
		SetupServer(level);
	}
	else
	{
		SetupClient(level);
	}
}

void GamePlayState::SetupServer(Level* level)
{
	Transform playerOneTransform;
	Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
	playerOne->InitializeController();

	level->AddPlayerToLevel(playerOne);
	SetCameraToPlayer(level->GetPlayerOne());

	gameConfig->networkConfig.server->SetPlayerConnectedCallback(
		[this, level](int playerID) {
			Transform playerTwoTransform;
			Player* playerTwo = manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");
			playerTwo->InitializeController();
			level->AddPlayerToLevel(playerTwo);
		});

	RegisterServerPacketHandlers(level);
}

void GamePlayState::SetupClient(Level* level)
{
	Transform playerOneTransform, playerTwoTransform;
	Player* playerOne = manager->AddPlayerToWorld(playerOneTransform, "playerOne");
	Player* playerTwo = manager->AddPlayerToWorld(playerTwoTransform, "playerTwo");

	playerOne->InitializeController();
	playerTwo->InitializeController();

	level->AddPlayerToLevel(playerTwo);
	SetCameraToPlayer(level->GetPlayerTwo());

	level->AddPlayerToLevel(playerOne);
	PositionInitialClientPlayer(level);

	RegisterClientPacketHandlers(level);
}

void GamePlayState::SetCameraToPlayer(Player* player)
{
	if (player)
	{
		Vector3 playerPosition = player->GetTransform().GetPosition();
		GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPosition(playerPosition);
	}
}

void GamePlayState::PositionInitialClientPlayer(Level* level)
{
	if (level->GetPlayerOne() && level->GetPlayerOne()->GetPlayerObject())
	{
		level->GetPlayerOne()->GetTransform().SetPosition(Vector3(10, -100, 10));
	}
}

void GamePlayState::RegisterServerPacketHandlers(Level* level)
{
	gameConfig->networkConfig.server->RegisterPacketHandler(Player_ID_Assignment, level);
	gameConfig->networkConfig.server->RegisterPacketHandler(Player_Position, level);
}

void GamePlayState::RegisterClientPacketHandlers(Level* level)
{
	gameConfig->networkConfig.client->RegisterPacketHandler(Player_Position, level);
	gameConfig->networkConfig.client->RegisterPacketHandler(Player_ID_Assignment, level);
}

void GamePlayState::LoadTextures()
{
	heartFilledTexture = GameBase::GetGameBase()->GetRenderer()->LoadTexture("heart_filled.png");
	heartEmptyTexture = GameBase::GetGameBase()->GetRenderer()->LoadTexture("heart_empty.png");
}

void GamePlayState::DrawHUDPanel() {
	// Set cursor to the top-left corner
	ImGui::SetCursorPos(ImVec2(25, 25));

	// Define the remaining lives and max number of hearts (e.g., 5 hearts)
	int remainingLives = 3; // Number of filled hearts
	int maxLives = 5;       // Maximum number of hearts to display

	GLuint texIDFilled = ((OGLTexture*)heartFilledTexture)->GetObjectID();  // Filled heart texture
	GLuint texIDUnfilled = ((OGLTexture*)heartEmptyTexture)->GetObjectID(); // Unfilled heart texture

	// Calculate the size of the heart icon
	ImVec2 heartSize = ImVec2(40, 40); // Size of each heart

	// Initial position for hearts
	ImVec2 heartPos = ImGui::GetCursorScreenPos(); // This gets the current position for the heart drawing

	// Loop through to draw the hearts
	for (int i = 0; i < maxLives; i++) {
		ImVec2 heartPosition = ImVec2(heartPos.x + (i * heartSize.x), heartPos.y); // Adjust position for each heart
		ImVec2 heartEndPos = ImVec2(heartPosition.x + heartSize.x, heartPosition.y + heartSize.y); // Set the end position based on heart size

		// Draw filled heart for remaining lives
		if (i < remainingLives) {
			ImGui::GetWindowDrawList()->AddImage((ImTextureID)texIDFilled, heartPosition, heartEndPos); // Draw filled heart
		}
		// Draw unfilled heart for remaining hearts
		else {
			ImGui::GetWindowDrawList()->AddImage((ImTextureID)texIDUnfilled, heartPosition, heartEndPos); // Draw unfilled heart
		}

		// Debugging: output the position of each heart
		std::cout << "Drawing heart at: " << heartPosition.x << ", " << heartPosition.y << std::endl;
	}

	// Set next cursor position for Timer text
	ImGui::SetCursorPos(ImVec2(25, 75)); // Set cursor to below the hearts

	// Display Timer
	ImGui::Text("Time: %.2f s", 20.0f);



}