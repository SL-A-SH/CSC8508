#include "GameOverState.h"
#include "GamePlayState.h"
#include "MenuState.h"

void GameOverState::OnAwake() {
	Window::GetWindow()->ShowOSPointer(true);
	DrawGameOverPanel();
}

PushdownState::PushdownResult GameOverState::OnUpdate(float dt, PushdownState** newState) {
	if (stateChangeAction) {
		stateChangeAction(newState);
		stateChangeAction = nullptr;
		return PushdownResult::Push;
	}
	return PushdownResult::NoChange;
}

void GameOverState::DrawGameOverPanel() {
	std::string title;

	switch (gameOverReason) {
	case GameOverReason::TimeUp:
		title = "Time's Up!";
		break;
	case GameOverReason::GameOver:
		title = "Game Over!";
		break;
	case GameOverReason::OutOfLives:
		title = "Out of Lives!";
		break;
	}

	std::vector<PanelButton> buttons = {
		{"Retry", [this]() {
			stateChangeAction = [](PushdownState** newState) { *newState = new GamePlayState(false, false); };
		}, 0.35f},
		{"Continue", [this]() {
			stateChangeAction = [](PushdownState** newState) { *newState = new MenuState(); };
		}, 0.55f}
	};

	ImGuiManager::DrawPanel(title, buttons);
}