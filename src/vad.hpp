#pragma once

#include "sherpa-onnx/c-api/c-api.h"
#include <vector>

namespace astro {

class VoiceActivityDetector {
public:
    VoiceActivityDetector();
    ~VoiceActivityDetector();

    bool detect(const std::vector<float>& samples, std::vector<float>& out);
private:
    int32_t m_window_size;
    SherpaOnnxVoiceActivityDetector *m_vad = nullptr;
};

}