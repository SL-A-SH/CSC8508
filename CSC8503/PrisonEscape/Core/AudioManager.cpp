#include "audiomanager.h"
#include <iostream>
#include <Include/AL/al.h>
#include <Include/AL/alc.h>
#include <vector>
#include <fstream>

AudioManager::AudioManager()
    : device(nullptr), context(nullptr) {
}

AudioManager::~AudioManager() {
    Shutdown();
}

bool AudioManager::Initialize() {
    // Open the default device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open an OpenAL device!" << std::endl;
        return false;
    }

    // Create a context for the device
    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "Failed to create or set OpenAL context!" << std::endl;
        alcCloseDevice(device);
        return false;
    }

    return true;
}

void AudioManager::Shutdown() {
    for (auto& sound : sounds) {
        alDeleteBuffers(1, &sound.second);
    }

    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }
    if (device) {
        alcCloseDevice(device);
    }
}

bool AudioManager::LoadSound(const std::string& filePath, const std::string& soundName) {
    ALuint buffer = LoadWavFile(filePath);
    if (buffer == 0) {
        std::cerr << "Failed to load sound file: " << filePath << std::endl;
        return false;
    }

    sounds[soundName] = buffer;
    return true;
}

void AudioManager::PlaySound(const std::string& soundName) {
    if (sounds.find(soundName) != sounds.end()) {
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, sounds[soundName]);
        alSourcePlay(source);
    }
    else {
        std::cerr << "Sound not found: " << soundName << std::endl;
    }
}

void AudioManager::StopSound(const std::string& soundName) {
    if (sounds.find(soundName) != sounds.end()) {
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, sounds[soundName]);
        alSourceStop(source);
        alDeleteSources(1, &source);
    }
    else {
        std::cerr << "Sound not found: " << soundName << std::endl;
    }
}

ALuint AudioManager::LoadWavFile(const std::string& filePath) {
    // For simplicity, let's assume this function loads a WAV file and returns an OpenAL buffer.
    // You can use an external library like stb_vorbis or stb_wav to load the WAV file and get audio data.
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open WAV file!" << std::endl;
        return 0;
    }

    // For simplicity, we'll assume the file is small enough and use raw data.
    std::vector<char> data((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), data.size(), 44100); // Assuming 44100Hz sample rate, mono format, etc.
    return buffer;
}