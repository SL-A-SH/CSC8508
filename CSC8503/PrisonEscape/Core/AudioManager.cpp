#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() : system(nullptr) {}

AudioManager::~AudioManager() {
    // Release all sounds and the FMOD system when the AudioManager is destroyed
    for (auto& soundPair : sounds) {
        soundPair.second->release();
    }
    system->close();
    system->release();
}

bool AudioManager::Initialize() {
    FMOD_RESULT result = FMOD::System_Create(&system);
    if (result != FMOD_OK) {
        std::cerr << "Failed to create FMOD system!" << std::endl;
        return false;
    }

    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cerr << "Failed to initialize FMOD!" << std::endl;
        return false;
    }

    return true;
}

void AudioManager::LoadSound(const std::string& filePath) {
    if (sounds.find(filePath) != sounds.end()) {
        std::cerr << "Sound already loaded: " << filePath << std::endl;
        return;
    }

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    if (result != FMOD_OK) {
        std::cerr << "Failed to load sound: " << filePath << std::endl;
        return;
    }

    sounds[filePath] = sound;
    std::cout << "Sound loaded: " << filePath << std::endl;
}

void AudioManager::PlaySound(const std::string& filePath, bool loop) {
    // Check if sound is loaded
    if (sounds.find(filePath) == sounds.end()) {
        std::cerr << "Sound not loaded: " << filePath << std::endl;
        return;
    }

    FMOD::Sound* sound = sounds[filePath];
    FMOD::Channel* channel = nullptr;

    // If we want to loop, set the loop flag in the sound creation
    FMOD_RESULT result = system->playSound(sound, nullptr, true, &channel);
    if (result != FMOD_OK) {
        std::cerr << "Failed to play sound: " << filePath << std::endl;
        return;
    }

    // Set looping if requested
    channel->setLoopCount(loop ? -1 : 0);
    channels[filePath] = channel;
}

void AudioManager::StopSound(const std::string& filePath) {
    // Check if the sound is playing
    if (channels.find(filePath) == channels.end()) {
        std::cerr << "Sound is not playing: " << filePath << std::endl;
        return;
    }

    FMOD::Channel* channel = channels[filePath];
    channel->stop();
    channels.erase(filePath);
    std::cout << "Stopped sound: " << filePath << std::endl;
}

void AudioManager::Update() {
    // This updates the FMOD system, typically called once per frame
    system->update();
}