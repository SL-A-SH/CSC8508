#include "MenuState.h"
#include "GamePlayState.h"
#include <iostream>


using namespace NCL;
using namespace CSC8503;

void MenuState::OnAwake()
{
	ImGuiIO& imguiIO = ImGui::GetIO();
	buttonFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 30.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	headerFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 60.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	imguiIO.Fonts->Build();

	GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawMainMenuPanel, this));
	Window::GetWindow()->ShowOSPointer(true);
}

PushdownState::PushdownResult MenuState::OnUpdate(float dt, PushdownState** newState)
{
	if (stateChangeAction) { // Check if the function pointer is set
		stateChangeAction(newState);
		stateChangeAction = nullptr; // Reset the function pointer
		return PushdownResult::Push;
	}

	return PushdownResult::NoChange;
}

void MenuState::DrawMainMenuPanel()
{
	ImVec2 windowSize = ImGui::GetWindowSize();


	ImGui::PushFont(headerFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .45f, windowSize.y * .10f));
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "RACECAR");
	ImGui::PopFont();


	ImGui::PushFont(buttonFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .25f));

	if (ImGui::Button("Single Player", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		stateChangeAction = [](PushdownState** newState) {	*newState = new GamePlayState();	};
		Window::GetWindow()->ShowOSPointer(false);
		// Unbind the current canvas function
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(nullptr);
	}

	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .45f));
	if (ImGui::Button("Multiplayer", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawMultiplayerPanel, this));
	}

	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .65f));
	if (ImGui::Button("Settings", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "I See you like touching buttons!!!! oh my my!");
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawSettingPanel, this));
	}

	ImGui::PopFont();
}

void MenuState::DrawSettingPanel()
{

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(headerFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .45f, windowSize.y * .10f));
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Settings");
	ImGui::PopFont();

	ImGui::PushFont(buttonFont);

	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .25f));
	if (ImGui::Button("Audio", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawAudioSettingPanel, this));
	}

	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .45f));
	if (ImGui::Button("Graphic", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
		ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawVideoSettingPanel, this));
	}

	ImGui::SetNextItemWidth(windowSize.x * .05f);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .65f));

	if (ImGui::Button("Back", ImVec2(windowSize.x * .3f, windowSize.y * .1f)))
	{
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawMainMenuPanel, this));
	}

	ImGui::PopFont();
}

void MenuState::DrawAudioSettingPanel()
{
	//GameBase::GetGameBase()->GetAudio()->masterVolume
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(headerFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .425f, windowSize.y * .10f));
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Audio Settings");
	ImGui::PopFont();

	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .36f));
	ImGui::Text("Master Volume"); // Draw the label first
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(windowSize.x * .5f, windowSize.y * .35f));

	ImGui::SetNextItemWidth(windowSize.x * .2f);
	ImGui::SliderInt("##", &volume, 0, 100);

	ImGui::SetNextItemWidth(windowSize.x * .05f);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .65f));

	if (ImGui::Button("Back", ImVec2(windowSize.x * .3f, windowSize.y * .1f)))
	{
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawSettingPanel, this));
	}
	std::cout << volume << std::endl;
}

void MenuState::DrawVideoSettingPanel()
{
	//GameBase::GetGameBase()->GetAudio()->masterVolume
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(headerFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .425f, windowSize.y * .10f));
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Video Settings");
	ImGui::PopFont();

	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .36f));
	ImGui::Text("Brightness");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(windowSize.x * .5f, windowSize.y * .35f));

	ImGui::SetNextItemWidth(windowSize.x * .2f);
	//Brighness slider update this later
	ImGui::SliderInt("##", &volume, 0, 100);

	ImGui::SetNextItemWidth(windowSize.x * .05f);
	ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .65f));

	if (ImGui::Button("Back", ImVec2(windowSize.x * .3f, windowSize.y * .1f)))
	{
		GameBase::GetGameBase()->GetRenderer()->SetImguiCanvasFunc(std::bind(&MenuState::DrawSettingPanel, this));
	}
	//std::cout << volume << std::endl;
}

void MenuState::DrawMultiplayerPanel()
{
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(headerFont);
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "World Rich Replied to Mails:");
	ImGui::PopFont();
}