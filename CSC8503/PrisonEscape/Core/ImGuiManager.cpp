#include "ImGuiManager.h"

using namespace NCL;
using namespace CSC8503;

ImFont* ImGuiManager::buttonFont = nullptr;
ImFont* ImGuiManager::headerFont = nullptr;

void ImGuiManager::Initialize() {
    ImGuiIO& imguiIO = ImGui::GetIO();
    buttonFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 30.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
    headerFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 60.0f, NULL, imguiIO.Fonts->GetGlyphRangesDefault());
    imguiIO.Fonts->Build();
}

void ImGuiManager::DrawPanel(
    const std::string& title,
    const std::vector<PanelButton>& buttons,
    const std::vector<PanelSlider>& sliders,
    std::function<void()> backCallback
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
}

void ImGuiManager::DrawHeader(const std::string& title) {
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