#pragma once
#include "PauseState.h"
#include "GamePlayState.h"
#include "MenuState.h"
#include <iostream>



#include "PrisonEscape/Core/ImGuiManager.h"

using namespace NCL;
using namespace CSC8503;

void PauseState::OnAwake() {

	GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseMenuPanel", [this]() { DrawPauseMenuPanel(); });
	Window::GetWindow()->ShowOSPointer(true);
}

PushdownState::PushdownResult PauseState::OnUpdate(float dt, PushdownState** newState) {
	if (stateChangeAction) {
		stateChangeAction(newState);
		stateChangeAction = nullptr;
		return PushdownResult::Pop;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
		stateChangeAction = [](PushdownState** newState) { *newState = nullptr; }; // Resumes the game
		return PushdownResult::Pop;
	}
	return PushdownResult::NoChange;
}

void PauseState::DrawPauseMenuPanel() {
	std::vector<PanelButton> buttons = {
		{"Resume", [this]() {
			stateChangeAction = [](PushdownState** newState) { *newState = nullptr; }; // Resumes the game
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseMenuPanel");
		}, 0.25f},
		{"Settings", [this]() {
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseSettingPanel", [this]() { DrawSettingPanel(); });
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseMenuPanel");
		}, 0.45f},
		{"Exit to Main Menu", [this]() {
			stateChangeAction = [](PushdownState** newState) { *newState = new MenuState(); }; // Goes back to main menu
		}, 0.65f}
	};

	ImGuiManager::DrawPanel("Pause Menu", buttons);
}

void PauseState::DrawSettingPanel() {
	std::vector<PanelButton> buttons = {
		{"Audio", [this]() {
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseAudioSettingPanel", [this]() { DrawAudioSettingPanel(); });
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseSettingPanel");
		}, 0.25f},
		{"Graphics", [this]() {
			GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseGraphicSettingPanel", [this]() { DrawVideoSettingPanel(); });
			GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseSettingPanel");
		}, 0.45f}
	};

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseMenuPanel", [this]() { DrawPauseMenuPanel(); });
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseSettingPanel");
		};

	ImGuiManager::DrawPanel("Settings", buttons, {}, backCallback);
}

void PauseState::DrawAudioSettingPanel() {
	std::vector<PanelSlider> sliders = { {"Master Volume", &volume, 0, 100, 0.36f} };

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("SettingPanel", [this]() {DrawSettingPanel();});
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("AudioSettingPanel");
		};

	ImGuiManager::DrawPanel("Audio Settings", {}, sliders, backCallback);
}

void PauseState::DrawVideoSettingPanel() {
	std::vector<PanelSlider> sliders = { {"Brightness", &brightness, 0, 100, 0.36f} };

	auto backCallback = [this]() {
		GameBase::GetGameBase()->GetRenderer()->AddPanelToCanvas("PauseSettingPanel", [this]() { DrawSettingPanel(); });
		GameBase::GetGameBase()->GetRenderer()->DeletePanelFromCanvas("PauseGraphicSettingPanel");
		};

	ImGuiManager::DrawPanel("Video Settings", {}, sliders, backCallback);
}
