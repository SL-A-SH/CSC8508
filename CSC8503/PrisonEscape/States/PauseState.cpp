#pragma once
#include "PauseState.h"
#include "GamePlayState.h"
#include "MenuState.h"
#include <iostream>

#include "PrisonEscape/Core/GameBase.h"

#include "PrisonEscape/Core/ImGuiManager.h"

using namespace NCL;
using namespace CSC8503;

void PauseState::OnAwake() {
	GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseMenuPanel", [this]() { DrawPauseMenuPanel(); });
	GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("HUDPanel");
	Window::GetWindow()->ShowOSPointer(true);
}

PushdownState::PushdownResult PauseState::OnUpdate(float dt, PushdownState** newState) {
	if (buttonClicked == ButtonClicked::Resume) {
		*newState = nullptr;
		return PushdownResult::Pop;
	}
	else if (buttonClicked == ButtonClicked::Settings) {
		return PushdownResult::NoChange;
	}
	else if (buttonClicked == ButtonClicked::Exit) {
		*newState = new MenuState;
		return PushdownResult::Push;
	}
	return PushdownResult::NoChange;
}

void PauseState::DrawPauseMenuPanel() {
	std::vector<PanelButton> buttons = {
		{"Resume", [this]() {
			//stateChangeAction = [](PushdownState** newState) { *newState = nullptr; }; // Resumes the game
			this->buttonClicked = ButtonClicked::Resume;
			//GameBase::GetGameBase()->GetStateMachine()->PopState();
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseMenuPanel");
		}, .32f,0.25f},
		{"Settings", [this]() {
			this->buttonClicked = ButtonClicked::Settings;
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseSettingPanel", [this]() { DrawSettingPanel(); });
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseMenuPanel");
		},.32f, 0.45f},
		{"Exit to Main Menu", [this]() {
			this->buttonClicked = ButtonClicked::Exit;
			//GameBase::GetGameBase()->GetStateMachine()->PushState(new MenuState);
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseMenuPanel");

		},.32f, 0.65f}
	};

	ImGuiManager::DrawPanel("Pause Menu", buttons);
}

void PauseState::DrawSettingPanel() {
	std::vector<PanelButton> buttons = {
		{"Audio", [this]() {
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseAudioSettingPanel", [this]() { DrawAudioSettingPanel(); });
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseSettingPanel");
		},0.10f, 0.35f},
		{"Graphics", [this]() {
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseGraphicSettingPanel", [this]() { DrawVideoSettingPanel(); });
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseSettingPanel");
		},0.55f, 0.35f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseMenuPanel", [this]() { DrawPauseMenuPanel(); });
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseSettingPanel");
		};

	ImGuiManager::DrawPanel("Settings", buttons, {}, backCallback);
}

void PauseState::DrawAudioSettingPanel() {
	std::vector<PanelSlider> sliders = { {"Master Volume", &volume, 0, 100,0.36f, 0.36f} };

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseSettingPanel", [this]() {DrawSettingPanel(); });
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseAudioSettingPanel");
		};

	ImGuiManager::DrawPanel("Audio Settings", {}, sliders, backCallback);
}

void PauseState::DrawVideoSettingPanel() {
	std::vector<PanelSlider> sliders = { {"Brightness", &brightness, 0, 100, 0.36f, 0.36f} };

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseSettingPanel", [this]() { DrawSettingPanel(); });
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseGraphicSettingPanel");
		};

	ImGuiManager::DrawPanel("Video Settings", {}, sliders, backCallback);
}
