#pragma once
#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Core/ImGuiManager.h"
namespace NCL {
	namespace CSC8503 {
		class MenuState;

		class GameSettingManager {

		public:
			GameSettingManager();
			~GameSettingManager();
			void SetVolume(int volume);
			void SetBrightness(int brightness);
			int GetVolume() const { return volume; }
			int GetBrightness() const { return brightness; }


			void DrawSettingPanel();

			friend class MenuState;
		private:
			int volume;
			int brightness;
		};
	}
}