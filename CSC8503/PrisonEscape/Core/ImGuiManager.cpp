#include "ImGuiManager.h"

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
		DrawButton(button.label, button.callback, button.verticalPosition);
	}

	// Draw sliders
	for (const auto& slider : sliders) {
		DrawSlider(slider.label, slider.value, slider.min, slider.max, slider.verticalPosition);
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
	ImGui::PushFont(headerFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.45f, windowSize.y * HEADER_VERTICAL_POS));
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "%s", title.c_str());
	ImGui::PopFont();
}

void ImGuiManager::DrawButton(const std::string& label, const std::function<void()>& callback, float verticalPos) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::PushFont(buttonFont);
	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.35f, windowSize.y * verticalPos));

	if (ImGui::Button(label.c_str(), ImVec2(windowSize.x * BUTTON_WIDTH_RATIO, windowSize.y * BUTTON_HEIGHT_RATIO))) {
		if (callback) {
			callback();
		}
	}

	ImGui::PopFont();
}

void ImGuiManager::DrawSlider(const std::string& label, int* value, int min, int max, float verticalPos) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.35f, windowSize.y * verticalPos));
	ImGui::Text("%s", label.c_str());
	ImGui::SameLine();

	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.5f, windowSize.y * verticalPos));
	ImGui::SetNextItemWidth(windowSize.x * 0.2f);
	ImGui::SliderInt("##", value, min, max);
}

void ImGuiManager::DrawBackButton(const std::function<void()>& callback) {
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::PushFont(buttonFont);
	ImGui::SetNextItemWidth(windowSize.x * 0.05f);
	ImGui::SetCursorPos(ImVec2(windowSize.x * 0.35f, windowSize.y * BACK_BUTTON_VERTICAL_POS));

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