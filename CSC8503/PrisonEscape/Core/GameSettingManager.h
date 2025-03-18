#pragma once
#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Core/ImGuiManager.h"

namespace NCL {
	namespace CSC8503 {
		class MenuState;

		class GameSettingManager {
		public:

			static GameSettingManager& Instance() {
				static GameSettingManager instance;
				return instance;
			}


			GameSettingManager(const GameSettingManager&) = delete;
			GameSettingManager& operator=(const GameSettingManager&) = delete;


			void SetVolume(int volume) {
				this->volume = volume;
			}
			void SetBrightness(int brightness) { this->brightness = brightness; }


			int GetVolume() const { return volume; }
			int GetBrightness() const { return brightness; }
		private:

			GameSettingManager() : volume(50), brightness(50) {}
			~GameSettingManager() {}

			int volume;
			int brightness;
		};
	}
}
