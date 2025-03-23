#include <portaudio.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

#include "player.hpp"

namespace astro {

void AudioPlayer::play() {
    if (isPlaying) {
        return;
    }

    PaError err;
    if (!stream) {
        err = Pa_OpenDefaultStream(&stream,
                                 0,          // no input channels
                                 1,          // mono output
                                 paFloat32,  // sample format
                                 44100,      // sample rate
                                 256,        // frames per buffer
                                 paCallback,
                                 this);
        if (err != paNoError) {
            throw std::runtime_error("Failed to open PortAudio stream: " + 
                                   std::string(Pa_GetErrorText(err)));
        }
    }

    position = 0;
    isPlaying = true;
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        isPlaying = false;
        throw std::runtime_error("Failed to start PortAudio stream: " + 
                               std::string(Pa_GetErrorText(err)));
    }
}

void AudioPlayer::stop() {
    if (!isPlaying) {
        return;
    }

    PaError err = Pa_StopStream(stream);
    if (err != paNoError) {
        throw std::runtime_error("Failed to stop PortAudio stream: " + 
                               std::string(Pa_GetErrorText(err)));
    }
    isPlaying = false;
}

bool AudioPlayer::playing() const {
    return isPlaying;
}

int AudioPlayer::paCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData) {
    AudioPlayer* player = (AudioPlayer*)userData;
    float* out = (float*)outputBuffer;
    
    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        if (player->position >= player->audioData.size()) {


            return paComplete;
        }
        *out++ = player->audioData[player->position++];
    }
    return paContinue;
}

AudioPlayer::AudioPlayer() : stream(nullptr), position(0), isPlaying(false) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error("PortAudio initialization failed: " + 
                               std::string(Pa_GetErrorText(err)));
    }
}

AudioPlayer::~AudioPlayer() {
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    Pa_Terminate();
}

} // namespace astro