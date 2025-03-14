#include "ImGuiManager.h"
#include "Texture.h"
#include "GameBase.h"
#include "PrisonEscape/States/MenuState.h"
using namespace NCL;
using namespace CSC8503;

ImFont* ImGuiManager::buttonFont = nullptr;
ImFont* ImGuiManager::headerFont = nullptr;
ImFont* ImGuiManager::footerFont = nullptr;
ImFont* ImGuiManager::messageFont = nullptr;


void ImGuiManager::Initialize() {

	ImGuiIO& imguiIO = ImGui::GetIO();
	buttonFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 30.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	headerFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 60.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	footerFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 30.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	messageFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 24.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
	imguiIO.Fonts->Build();
}

void ImGuiManager::DrawPanel(
	const std::string& title,
	const std::vector<PanelButton>& buttons,
	const std::vector<PanelSlider>& sliders,
	std::function<void()> backCallback,
	const std::string& footer
) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	// Draw header
	DrawHeader(title);

	// Draw buttons
	for (const auto& button : buttons) {
		DrawButton(button.label, button.callback, button.xPosition, button.yPosition);
	}

	// Draw sliders
	for (const auto& slider : sliders) {
		DrawSlider(slider.label, slider.value, slider.min, slider.max, slider.xPosition, slider.yPosition);
	}

	// Draw back button if callback provided
	if (backCallback) {
		DrawBackButton(backCallback);
	}

	// Draw footer
	DrawFooter(footer);
}

void ImGuiManager::DrawMessagePanel(
	const std::string& title,
	const std::string& message,
	const ImVec4& messageColor,
	std::function<void()> cancelCallback
) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	// Draw header
	DrawHeader(title);

	// Draw message in center of screen
	ImGui::PushFont(messageFont);

	// Calculate text size to center it
	ImVec2 textSize = ImGui::CalcTextSize(message.c_str());
	ImGui::SetCursorPos(ImVec2(
		(windowSize.x - textSize.x) * 0.5f,
		windowSize.y * 0.4f
	));

	ImGui::TextColored(messageColor, "%s", message.c_str());
	ImGui::PopFont();

	// Draw loading spinner/animation
	float time = ImGui::GetTime();
	float radius = 25.0f;
	ImVec2 center = ImVec2(windowSize.x * 0.5f, windowSize.y * 0.5f);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Draw spinning circle
	const int num_segments = 12;
	const float PI = 3.14159265358979323846f;
	float start = time * 5.0f;

	for (int i = 0; i < num_segments; i++) {
		float a = start + (i * 2 * PI / num_segments);
		float b = start + ((i + 1) * 2 * PI / num_segments);
		float alpha = 1.0f - (float)i / (float)num_segments;

		draw_list->AddLine(
			ImVec2(center.x + cosf(a) * radius, center.y + sinf(a) * radius),
			ImVec2(center.x + cosf(b) * radius, center.y + sinf(b) * radius),
			ImGui::GetColorU32(ImVec4(messageColor.x, messageColor.y, messageColor.z, alpha)),
			3.0f
		);
	}

	// Draw cancel button if provided
	if (cancelCallback) {
		ImGui::PushFont(buttonFont);
		ImGui::SetCursorPos(ImVec2(windowSize.x * 0.35f, windowSize.y * 0.65f));

		if (ImGui::Button("Cancel", ImVec2(windowSize.x * BUTTON_WIDTH_RATIO, windowSize.y * BUTTON_HEIGHT_RATIO))) {
			cancelCallback();
		}

		ImGui::PopFont();
	}
}

void ImGuiManager::DrawHeader(const std::string& title) {
	if (title.empty()) {
		return;
	}

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 textSize = ImGui::CalcTextSize(title.c_str());

	// Keep it at the top and center it horizontally
	ImVec2 textPos = ImVec2((windowSize.x - textSize.x) * 0.47f, windowSize.y * 0.05f); // Adjust vertical position as needed

	ImGui::PushFont(headerFont);
	ImGui::SetCursorPos(textPos);
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "%s", title.c_str());
	ImGui::PopFont();


}

void ImGuiManager::DrawButton(const std::string& label, const std::function<void()>& callback, float xPos, float yPos) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::PushFont(buttonFont);
	// Set cursor position using the new x and y positions
	ImGui::SetCursorPos(ImVec2(windowSize.x * xPos, windowSize.y * yPos));
	//nEED TO FIND A WAY TO LOAD tEXTURE HERE THIS WON'T WORL
	/*Texture* tex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("button.png");
	GLuint texID = ((OGLTexture*)tex)->GetObjectID();*/
	if (ImGui::Button(label.c_str(), ImVec2(windowSize.x * BUTTON_WIDTH_RATIO, windowSize.y * BUTTON_HEIGHT_RATIO))) {
		if (callback) {
			callback();
		}
	}



	ImGui::PopFont();
}


void ImGuiManager::DrawSlider(const std::string& label, int* value, int min, int max, float horizontalPos, float verticalPos) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	// Position the label
	ImGui::SetCursorPos(ImVec2(windowSize.x * horizontalPos, windowSize.y * verticalPos));
	ImGui::Text("%s", label.c_str());

	// Align the slider right next to the label, or adjust position if needed
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(windowSize.x * (horizontalPos + 0.15f), windowSize.y * verticalPos)); // Slight offset for slider
	ImGui::SetNextItemWidth(windowSize.x * 0.2f);
	ImGui::SliderInt(("##" + label).c_str(), value, min, max);
}


void ImGuiManager::DrawBackButton(const std::function<void()>& callback) {
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(buttonFont);

	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.32f, windowSize.y * .65f));

	if (ImGui::Button("Back", ImVec2(windowSize.x * BUTTON_WIDTH_RATIO, windowSize.y * BUTTON_HEIGHT_RATIO))) {
		if (callback) {
			callback();
		}
	}

	ImGui::PopFont();
}

void ImGuiManager::DrawFooter(const std::string& text) {
	if (text.empty())
	{
		return;
	}
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::PushFont(buttonFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.35f, windowSize.y * 0.85f));
	ImGui::Text("%s", text.c_str());
	ImGui::PopFont();
}