#pragma once

#include <portaudio.h>
#include <string>
#include <vector>

namespace astro {

class AudioPlayer {
private:
    PaStream* stream;
    std::vector<float> audioData;
    size_t position;
    bool isPlaying;

    static int paCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData);

public:
    AudioPlayer();
    ~AudioPlayer();
    void play(const std::vector<uint8_t>& wavData);
};

} // namespace astro
