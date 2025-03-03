#include "MenuState.h"
#include "GamePlayState.h"
#include <iostream>

#include "PrisonEscape/Core/ImGuiManager.h"

using namespace NCL;
using namespace CSC8503;

void MenuState::OnAwake() {
	ImGuiManager::Initialize();
	GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawMainMenuPanel, this));

	Window::GetWindow()->ShowOSPointer(true);
}

PushdownState::PushdownResult MenuState::OnUpdate(float dt, PushdownState** newState) {
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
			GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawMainMenuPanel, this));
			stateChangeAction = [](PushdownState** newState) { *newState = new GamePlayState(false, false); };

		}, 0.25f},
		{"Multiplayer", [this]() {
			GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawMainMenuPanel, this));
			GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawMultiplayerPanel, this));
		}, 0.45f},
		{"Settings", [this]() {
			GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawMainMenuPanel, this));
			GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawSettingPanel, this));
		}, 0.65f}
	};

	ImGuiManager::DrawPanel("PRISON ESCAPE", buttons);
}

void MenuState::DrawSettingPanel() {
	std::vector<PanelButton> buttons = {
		{"Audio", [this]() {
				GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawSettingPanel, this));
			GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawAudioSettingPanel, this));
		}, 0.25f},
		{"Graphic", [this]() {
				GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawSettingPanel, this));
			GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawVideoSettingPanel, this));
		}, 0.45f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawSettingPanel, this));
		GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawMainMenuPanel, this));
		};

	ImGuiManager::DrawPanel("Settings", buttons, {}, backCallback);
}

void MenuState::DrawAudioSettingPanel() {
	std::vector<PanelSlider> sliders = {
		{"Master Volume", &volume, 0, 100, 0.36f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawAudioSettingPanel, this));
		GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawSettingPanel, this));
		};

	ImGuiManager::DrawPanel("Audio Settings", {}, sliders, backCallback);
}

void MenuState::DrawVideoSettingPanel() {
	std::vector<PanelSlider> sliders = {
		{"Brightness", &brightness, 0, 100, 0.36f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawVideoSettingPanel, this));
		GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawSettingPanel, this));
		};

	ImGuiManager::DrawPanel("Video Settings", {}, sliders, backCallback);
}

void MenuState::DrawMultiplayerPanel() {
	std::vector<PanelButton> buttons = {
		{"Host", [this]() {
			stateChangeAction = [](PushdownState** newState) { *newState = new GamePlayState(true, true); };
			Window::GetWindow()->ShowOSPointer(false);
			GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawMultiplayerPanel, this));
		}, 0.25f},
		{"Join", [this]() {
			stateChangeAction = [](PushdownState** newState) { *newState = new GamePlayState(true, false); };
			Window::GetWindow()->ShowOSPointer(false);
			GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawMultiplayerPanel, this));
		}, 0.45f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction(std::bind(&MenuState::DrawMultiplayerPanel, this));
		GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction(std::bind(&MenuState::DrawMainMenuPanel, this));
		};

	ImGuiManager::DrawPanel("Multiplayer", buttons, {}, backCallback);
}