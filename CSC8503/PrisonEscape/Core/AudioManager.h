#pragma once
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Include/AL/al.h>
#include <Include/AL/alc.h>
#include <string>
#include <unordered_map>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool Initialize();
    void Shutdown();
    
    bool LoadSound(const std::string& filePath, const std::string& soundName);
    void PlaySound(const std::string& soundName);
    void StopSound(const std::string& soundName);

private:
    ALCdevice* device;
    ALCcontext* context;
    std::unordered_map<std::string, ALuint> sounds;
    
    ALuint LoadWavFile(const std::string& filePath);
};

#endif // AUDIOMANAGER_H