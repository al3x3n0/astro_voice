#include <portaudio.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

#include "player.hpp"

namespace astro {

int AudioPlayer::paCallback(const void* inputBuffer, void* outputBuffer,
               unsigned long framesPerBuffer,
               const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags,
               void* userData) {

    AudioPlayer* player = static_cast<AudioPlayer*>(userData);
    float* out = static_cast<float*>(outputBuffer);
    
    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        if (player->position >= player->audioData.size()) {
            player->isPlaying = false;
            player->position = 0;
            return paComplete;
        }
        out[i] = player->audioData[player->position++];
    }
    return paContinue;
}

AudioPlayer::AudioPlayer() : stream(nullptr), isPlaying(false), position(0) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error("Failed to initialize PortAudio: " + 
                               std::string(Pa_GetErrorText(err)));
    }
}

AudioPlayer::~AudioPlayer() {
    if (stream) {
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
}

void AudioPlayer::play(const std::vector<uint8_t>& wavData) {
    // Convert new chunk to float format
    std::vector<float> newSamples;
    newSamples.reserve((wavData.size() - 44) / 2);  // Assuming 16-bit audio
    const int16_t* samples = reinterpret_cast<const int16_t*>(wavData.data() + 44);
    for (size_t i = 0; i < (wavData.size() - 44) / 2; i++) {
        newSamples.push_back(samples[i] / 16384.0f);
    }
    play(newSamples);
}

void AudioPlayer::play(const std::vector<float>& samples) {
    // If currently playing, append to existing audio
    if (isPlaying) {
        printf("Appending to existing audio\n");
        // Keep remaining unplayed audio
        std::vector<float> remainingAudio(audioData.begin() + position, audioData.end());
        // Append new samples
        remainingAudio.insert(remainingAudio.end(), samples.begin(), samples.end());
        audioData = remainingAudio;
        position = 0;
    } else {
        printf("Starting fresh with new samples\n");
        // Start fresh with new samples
        audioData = samples;
        position = 0;
        if (stream) {
            Pa_CloseStream(stream);
            stream = nullptr;
        }
    }

    // Initialize stream if needed
    if (!stream) {
        PaError err = Pa_OpenDefaultStream(&stream,
                                         0,          // no input channels
                                         1,          // mono output
                                         paFloat32,  // sample format
                                         22050,      // sample rate
                                         256,        // frames per buffer
                                         paCallback,
                                         this);
        if (err != paNoError) {
            throw std::runtime_error("Failed to open PortAudio stream: " + 
                                   std::string(Pa_GetErrorText(err)));
        }
    }

    // Start playback if not already playing
    if (!isPlaying) {
        isPlaying = true;
        PaError err = Pa_StartStream(stream);
        if (err != paNoError) {
            isPlaying = false;
            throw std::runtime_error("Failed to start PortAudio stream: " + 
                                   std::string(Pa_GetErrorText(err)));
        }
    }
}

} // namespace astro