#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <fmod.hpp>
#include <string>
#include <unordered_map>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool Initialize();
    void LoadSound(const std::string& filePath);
    void PlaySound(const std::string& filePath, bool loop = false);
    void StopSound(const std::string& filePath);
    void Update();

private:
    FMOD::System* system;
    std::unordered_map<std::string, FMOD::Sound*> sounds;
    std::unordered_map<std::string, FMOD::Channel*> channels;
};

#endif // AUDIOMANAGER_H