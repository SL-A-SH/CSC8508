#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once

#include "imgui/imgui.h"
#include "Level.h"
#include "../CSC8503/PrisonEscape/Core/GameBase.h"

using namespace NCL;

class SampleLevel : public Level
{
public:
	SampleLevel() :controller(*Window::GetKeyboard(), *Window::GetMouse())
	{

		GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetController(controller);

		controller.MapAxis(0, "Sidestep");
		controller.MapAxis(1, "UpDown");
		controller.MapAxis(2, "Forward");

		controller.MapAxis(3, "XLook");
		controller.MapAxis(4, "YLook");


	}
	~SampleLevel();

	void Init() override
	{
		Level::Init();
	}

	void Update(float dt) override
	{
		std::cout << "SampleLevel Update" << std::endl;
		GameBase::GetGameBase()->GetWorld()->GetMainCamera().UpdateCamera(dt);
	}


	void DrawPause()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(windowSize.x * .7f, windowSize.y * .05f));
		if (ImGui::Button("Pause", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
			ImGui::PushFont(mHeaderFont);
			ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "I See you like touching buttons!!!! oh my my!");
			ImGui::PopFont();
		}
	}



	ImFont* mHeaderFont = nullptr;

	KeyboardMouseController controller;

};

