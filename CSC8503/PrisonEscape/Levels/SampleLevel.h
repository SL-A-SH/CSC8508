#pragma once

#include "Vector.h"
#include "imgui/imgui.h"
#include "Level.h"
#include "../CSC8503/PrisonEscape/Core/GameBase.h"

using namespace NCL;
using namespace CSC8503;

class SampleLevel : public Level
{
public:
	SampleLevel();
	~SampleLevel();

	void Init() override;

	void Update(float dt) override;

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
	GameObject* CreateRandomColorCube(Vector3 position);
};

