#pragma once

#include "sherpa-onnx/c-api/c-api.h"
#include <vector>
#include "stt_base.hpp"

namespace astro {

class ZipformerSTT : public STTBase {
public:
    ZipformerSTT();
    ~ZipformerSTT();

    std::string transcribe(const std::vector<float>& samples);
private:
    int m_sample_rate = 16000;
    const SherpaOnnxOfflineRecognizer *m_recognizer = nullptr;
};

}
