#include "MenuState.h"
#include "GamePlayState.h"
#include <iostream>

#include "PrisonEscape/Core/ImGuiManager.h"

using namespace NCL;
using namespace CSC8503;

MenuState::MenuState() :
	isConnecting(false),
	connectionStage(ConnectionStage::None),
	networkAsServer(false),
	connectionTimer(0.0f),
	connectionAttempt(0),
	gameConfig(nullptr)
{

}

MenuState::~MenuState()
{
	if (gameConfig)
	{
		delete gameConfig;
	}
}

void MenuState::OnAwake() {
	ImGuiManager::Initialize();
	GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("MainMenuPanel", [this]() {	DrawMainMenuPanel();});


	Window::GetWindow()->ShowOSPointer(true);
}

PushdownState::PushdownResult MenuState::OnUpdate(float dt, PushdownState** newState)
{
	// Connection Process Handling
	if (isConnecting)
	{
		connectionTimer += dt;

		switch (connectionStage)
		{
		case ConnectionStage::Creating:
			if (connectionTimer >= 1.5f) {
				connectionStage = ConnectionStage::Connecting;
				connectionTimer = 0.0f;

				GameConfigManager* gameConfig = new GameConfigManager();
				gameConfig->networkConfig.isMultiplayer = true;
				gameConfig->networkConfig.isServer = networkAsServer;
				gameConfig->InitNetwork();

				if (networkAsServer) {
					try {
						gameConfig->CreateServer();

						this->gameConfig = gameConfig;

						connectionStage = ConnectionStage::Success;
						connectionTimer = 0.0f;
					}
					catch (std::exception& e) {
						delete gameConfig;
						this->gameConfig = nullptr;

						connectionStage = ConnectionStage::Failed;
						connectionTimer = 0.0f;
					}
				}
				else {
					try {
						gameConfig->CreateClient();
						this->gameConfig = gameConfig;

						connectionStage = ConnectionStage::Connecting;
						connectionTimer = 0.0f;
					}
					catch (std::exception& e) {
						delete gameConfig;
						this->gameConfig = nullptr;

						connectionStage = ConnectionStage::Failed;
						connectionTimer = 0.0f;
					}
				}
			}
			break;

		case ConnectionStage::Connecting:
			if (networkAsServer) {
				// Server is already in "success" state from previous step
			}
			else {
				if (connectionTimer >= 1.0f) {
					connectionTimer = 0.0f;

					if (gameConfig) {
						bool connected = false;

						try {
							if (gameConfig->networkConfig.client->Connect(127, 0, 0, 1, NetworkBase::GetDefaultPort())) {
								connected = true;
								connectionStage = ConnectionStage::Success;
							}
							else {
								connectionAttempt++;

								if (connectionAttempt > 5) {
									connectionStage = ConnectionStage::Failed;
								}
							}
						}
						catch (std::exception& e) {
							connectionStage = ConnectionStage::Failed;
						}
					}
					else {
						// No gameConfig, this shouldn't happen
						connectionStage = ConnectionStage::Failed;
					}
				}
			}
			break;

		case ConnectionStage::Success:
			if (connectionTimer >= 2.0f) {
				isConnecting = false;

				stateChangeAction = [this](PushdownState** newState) {
					*newState = new GamePlayState(true, networkAsServer);
					//GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(nullptr);
					if (this->gameConfig) {
						dynamic_cast<GamePlayState*>(*newState)->SetGameConfig(this->gameConfig);
						this->gameConfig = nullptr; // Transfer ownership
					}
					};

				connectionStage = ConnectionStage::None;
			}
			break;

		case ConnectionStage::Failed:
			if (connectionTimer >= 3.0f) {
				isConnecting = false;
				connectionStage = ConnectionStage::None;

				if (gameConfig) {
					delete gameConfig;
					gameConfig = nullptr;
				}

				//GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("MultiplayerPanel", [this]() {Draw();});
			}
			break;

		default:
			break;
		}
	}

	if (stateChangeAction) {
		stateChangeAction(newState);
		stateChangeAction = nullptr;
		return PushdownResult::Push;
	}
	return PushdownResult::NoChange;
}

void MenuState::DrawMainMenuPanel() {
	std::vector<PanelButton> buttons = {
		{"Single Player", [this]() {
			std::cout << "Single Player" << std::endl;
			stateChangeAction = [](PushdownState** newState) { *newState = new GamePlayState(false, false); };
		}, 0.25f},
		{"Multiplayer", [this]() {
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("MainMenuPanel");
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("MultiplayerPanel", [this]() {DrawMultiplayerPanel();});
		}, 0.45f},
		{"Settings", [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("SettingPanel", [this]() {DrawSettingPanel();});
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("MainMenuPanel");
		}, 0.65f}
	};

	ImGuiManager::DrawPanel("PRISON ESCAPE", buttons);
}

void MenuState::DrawSettingPanel() {
	std::vector<PanelButton> buttons = {
		{"Audio", [this]() {
				GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("AudioSettingPanel", [this]() {DrawAudioSettingPanel();});
				GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("SettingPanel");
		}, 0.25f},
		{"Graphic", [this]() {
				GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("GraphicSettingPanel", [this]() {DrawVideoSettingPanel();});
				GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("SettingPanel");
		}, 0.45f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("MainMenuPanel", [this]() {DrawMainMenuPanel();});
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("SettingPanel");
		};

	ImGuiManager::DrawPanel("Settings", buttons, {}, backCallback);
}

void MenuState::DrawAudioSettingPanel() {
	std::vector<PanelSlider> sliders = {
		{"Master Volume", &volume, 0, 100, 0.36f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("SettingPanel", [this]() {DrawSettingPanel();});
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("AudioSettingPanel");
		};

	ImGuiManager::DrawPanel("Audio Settings", {}, sliders, backCallback);
}

void MenuState::DrawVideoSettingPanel() {
	std::vector<PanelSlider> sliders = {
		{"Brightness", &brightness, 0, 100, 0.36f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("SettingPanel", [this]() {DrawSettingPanel();});
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("VideSettingPanel");
		};

	ImGuiManager::DrawPanel("Video Settings", {}, sliders, backCallback);
}

void MenuState::DrawMultiplayerPanel() {
	std::vector<PanelButton> buttons = {
		{"Host", [this]() {
			networkAsServer = true;
			StartServerProcess();
		}, 0.25f},
		{"Join", [this]() {
			networkAsServer = false;
			StartClientProcess();
		}, 0.45f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("MainMenuPanel", [this]() {DrawMainMenuPanel();});
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("MultiplayerPanel");
		};
	ImGuiManager::DrawPanel("Multiplayer", buttons, {}, backCallback);


}

void MenuState::StartServerProcess()
{
	connectionStage = ConnectionStage::Creating;
	connectionTimer = 0.0f;
	isConnecting = true;

	GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("ConnectionPanel", [this]() {DrawConnectionMessagePanel();});
}

void MenuState::StartClientProcess()
{
	connectionStage = ConnectionStage::Creating;
	connectionAttempt = 1;
	connectionTimer = 0.0f;
	isConnecting = true;

	GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("ConnectionPanel", [this]() {DrawConnectionMessagePanel();});
}

void MenuState::DrawConnectionMessagePanel()
{
	std::string title;
	std::string message;
	ImVec4 messageColor(1, 1, 1, 1);

	auto cancelCallback = [this]() {
		isConnecting = false;
		connectionStage = ConnectionStage::None;
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("MultiplayerPanel", [this]() {DrawMultiplayerPanel();});
		};

	// Determine message based on current stage
	if (networkAsServer) {
		title = "Creating Server";

		switch (connectionStage) {
		case ConnectionStage::Creating:
			message = "Initializing server...";
			messageColor = ImVec4(1, 1, 0, 1); // Yellow
			break;

		case ConnectionStage::Connecting:
			message = "Setting up network...";
			messageColor = ImVec4(1, 1, 0, 1); // Yellow
			break;

		case ConnectionStage::Success:
			message = "Server created successfully!";
			messageColor = ImVec4(0, 1, 0, 1); // Green
			break;

		case ConnectionStage::Failed:
			message = "Server creation failed.";
			messageColor = ImVec4(1, 0, 0, 1); // Red
			break;

		default:
			message = "Unknown status";
			break;
		}
	}
	else {
		title = "Connecting to Server";

		switch (connectionStage) {
		case ConnectionStage::Creating:
			message = "Initializing connection...";
			messageColor = ImVec4(1, 1, 0, 1); // Yellow
			break;

		case ConnectionStage::Connecting:
			message = "Connection attempt " + std::to_string(connectionAttempt) + " of 5...";
			messageColor = ImVec4(1, 1, 0, 1); // Yellow
			break;

		case ConnectionStage::Success:
			message = "Connected to server successfully!";
			messageColor = ImVec4(0, 1, 0, 1); // Green
			break;

		case ConnectionStage::Failed:
			message = "Connection failed after multiple attempts.";
			messageColor = ImVec4(1, 0, 0, 1); // Red
			break;

		default:
			message = "Unknown status";
			break;
		}
	}

	// Don't show cancel button if we've already succeeded or failed
	if (connectionStage == ConnectionStage::Success || connectionStage == ConnectionStage::Failed) {
		ImGuiManager::DrawMessagePanel(title, message, messageColor);
	}
	else {
		ImGuiManager::DrawMessagePanel(title, message, messageColor, cancelCallback);
	}
}