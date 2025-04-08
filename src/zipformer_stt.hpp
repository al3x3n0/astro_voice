#pragma once

#include <vector>
#include <string>
#include <sherpa-onnx/c-api/c-api.h>
#include "stt_base.hpp"

namespace astro {

class ZipformerSTT : public STTBase {
public:
    ZipformerSTT();
    ~ZipformerSTT();

    std::string transcribe(const std::vector<float>& samples);

private:
    const SherpaOnnxOfflineRecognizer *m_recognizer;
    int m_sample_rate = 16000;

    // Configuration strings
    std::string m_encoder_path;
    std::string m_decoder_path;
    std::string m_joiner_path;
    std::string m_tokens_path;
};

} // namespace astro 