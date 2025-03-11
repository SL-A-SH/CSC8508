#pragma once

#include "Vector.h"
#include "imgui/imgui.h"
#include "Level.h"
#include "../CSC8503/PrisonEscape/Core/GameBase.h"
#include "../CSC8503/PrisonEscape/Scripts/puzzle/Button.h"

using namespace NCL;

class LevelOne : public Level
{
public:
	LevelOne();
	~LevelOne();

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

protected:
	Button* button;
	GameObject* box;

private:
	GameObject* AddWallToWorld(Vector3 wallSize, const Vector3& position);
	GameObject* AddObstacleToWorld(Vector3 size, const Vector3& position);
	GameObject* AddBoxToWorld(const Vector3& position, Vector3 dimensions, const std::string name, float inverseMass = 10.0f);
	GameObject* AddRampToWorld(const Vector3& position, Vector3 dimensions, float x, float y, float z);
	// maybe add levelComplete boolean into level.cpp to keep track of level progress
	void openExit();
};