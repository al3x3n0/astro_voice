#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "portaudio.h"  // NOLINT
#include "kwspotter.hpp"
#include "vad.hpp"
#include "paraformer.hpp"
#include "audio_queue.hpp"

namespace astro {

class AudioStream {
public:
    AudioStream(std::shared_ptr<AudioQueue> audio_queue);
    ~AudioStream();

    void start();
    int OnRecord(const std::vector<float>& data);

private:
    int m_interval = 500;
    int m_frameSize = 1280;
    PaStreamParameters m_params;
    PaStream* m_stream = nullptr;
    std::shared_ptr<AudioQueue> m_audio_queue;
};

}
