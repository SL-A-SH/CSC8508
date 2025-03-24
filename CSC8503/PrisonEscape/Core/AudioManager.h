#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "../FMODCoreAPI/includes/fmod.hpp"

namespace NCL {
	namespace CSC8503 {
		class AudioManager {
		public:
			AudioManager();
			~AudioManager();

			// Initialize FMOD system
			bool Initialize();

			// Load a sound into the system
			void LoadSound(const std::string& filePath);

			// Play a sound
			void PlaySound(const std::string& filePath, bool loop = false);

			// Stop a sound
			void StopSound(const std::string& filePath);

			// Update the FMOD system (must be called periodically)
			void Update();

			void PrintOutputDevices();
			void SelectOutputDevice(int driverIndex);
		private:
			FMOD::System* system;
			std::unordered_map<std::string, FMOD::Sound*> sounds;
			std::unordered_map<std::string, FMOD::Channel*> channels;

		};
	}
}
#endif // AUDIOMANAGER_H