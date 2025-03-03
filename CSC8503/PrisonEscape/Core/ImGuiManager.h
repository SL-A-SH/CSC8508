#pragma once
#include "imgui/imgui.h"
#include <string>
#include <vector>
#include <functional>

namespace NCL {
	namespace CSC8503 {
		struct PanelButton {
			std::string label;
			std::function<void()> callback;
			float verticalPosition; // Position from 0.0 to 1.0
		};

		struct PanelSlider {
			std::string label;
			int* value;
			int min;
			int max;
			float verticalPosition; // Position from 0.0 to 1.0
		};

		class ImGuiManager {
		public:
			static void Initialize();
			static ImFont* GetButtonFont() { return buttonFont; }
			static ImFont* GetHeaderFont() { return headerFont; }

			static void DrawPanel(
				const std::string& title = "",
				const std::vector<PanelButton>& buttons = {},
				const std::vector<PanelSlider>& sliders = {},
				std::function<void()> backCallback = nullptr, const std::string& footer = ""
			);

		private:
			static void DrawHeader(const std::string& title);
			static void DrawButton(const std::string& label, const std::function<void()>& callback, float verticalPos);
			static void DrawSlider(const std::string& label, int* value, int min, int max, float verticalPos);
			static void DrawBackButton(const std::function<void()>& callback);
			static void DrawFooter(const std::string& text);

			static ImFont* buttonFont;
			static ImFont* headerFont;
			static ImFont* footerFont;

			static constexpr float BUTTON_WIDTH_RATIO = 0.3f;
			static constexpr float BUTTON_HEIGHT_RATIO = 0.1f;
			static constexpr float HEADER_VERTICAL_POS = 0.1f;
			static constexpr float BACK_BUTTON_VERTICAL_POS = 0.65f;
		};
	}
}