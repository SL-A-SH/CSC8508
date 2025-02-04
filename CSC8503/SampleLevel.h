#pragma once

#include "imgui/imgui.h"
class SampleLevel : public Level
{
public:
    SampleLevel()
    {
        std::cout << "SampleLevel" << std::endl;
        ImGuiIO& imguiIO = ImGui::GetIO();

        //second parameter is size
        mHeaderFont = imguiIO.Fonts->AddFontFromFileTTF("../Assets/Fonts/BebasNeue-Regular.ttf", 20.0f);
        imguiIO.Fonts->Build();
        GameBase::GetRenderer()->SetImguiCanvasFunc(std::bind(&SampleLevel::DrawCanvas,this));
    }
    ~SampleLevel();

    void DrawCanvas()
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGui::SetCursorPos(ImVec2(windowSize.x * .35f, windowSize.y * .55f));
        if (ImGui::Button("Single-player", ImVec2(windowSize.x * .3f, windowSize.y * .1f))) {
            ImGui::PushFont(mHeaderFont);
            ImGui::SetCursorPos(ImVec2(windowSize.x * .2f, windowSize.y * .1f));
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "I See you like touching buttons!!!! oh my my!");
            ImGui::PopFont();
        }
    }

    ImFont* mHeaderFont = nullptr;

};