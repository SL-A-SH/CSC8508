#include "MenuState.h"
#include "GamePlayState.h"
#include <iostream>


using namespace NCL;
using namespace CSC8503;

void MenuState::OnAwake()
{
	ImGuiIO& imguiIO = ImGui::GetIO();
	buttonFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 30.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	imguiIO.Fonts->Build();
	GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawCanvas, this));

	Window::GetWindow()->ShowOSPointer(true);
}

PushdownState::PushdownResult MenuState::OnUpdate(float dt, PushdownState** newState)
{
	if (stateChangeAction) { // Check if the function pointer is set
		stateChangeAction(newState);
		stateChangeAction = nullptr; // Reset the function pointer
		return PushdownResult::Push;
	}
	std::cout << "Drawing Canvas" << std::endl;
	return PushdownResult::NoChange;
}

void MenuState::DrawCanvas()
{

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(buttonFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .25f));
	if (ImGui::Button("Single Player", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		stateChangeAction = [](PushdownState** newState) {
			*newState = new GamePlayState();
			};
		Window::GetWindow()->ShowOSPointer(false);
		// Unbind the current canvas function
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(nullptr);

	}
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .45f));
	if (ImGui::Button("Multiplayer", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {

	}
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .65f));
	if (ImGui::Button("Settings", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "I See you like touching buttons!!!! oh my my!");
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawOptions, this));
	}
	ImGui::PopFont();
}

void MenuState::DrawOptions()
{

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(buttonFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .55f));
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Settings");
	if (ImGui::Button("Audio", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
		//GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawOptions, this));

	}
	if (ImGui::Button("Graphic", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
		//GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawOptions, this));

	}
	ImGui::PopFont();
}