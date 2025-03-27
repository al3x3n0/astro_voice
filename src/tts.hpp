#pragma once

#include <string>
#include <vector>
#include "sherpa-onnx/c-api/c-api.h"
#include "audio_queue.hpp"

namespace astro {

class TTS {
public:
    TTS();
    ~TTS();
    AudioData speak(const std::string& text);
private:
    const SherpaOnnxOfflineTts *m_tts = nullptr;
};

} // namespace astro
