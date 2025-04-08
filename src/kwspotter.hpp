#pragma once

#include <vector>
#include <string>
#include <sherpa-onnx/c-api/c-api.h>

namespace astro {

class KWSpotter {
public:
    KWSpotter(int sample_rate);
    ~KWSpotter();

    bool detect(const std::vector<float>& samples);

private:
    int m_sample_rate;
    const SherpaOnnxKeywordSpotter *m_kws;
    const SherpaOnnxOnlineStream *m_stream;

    // Configuration strings
    std::string m_encoder_path;
    std::string m_decoder_path;
    std::string m_joiner_path;
    std::string m_tokens_path;
    std::string m_keywords_file;
};

} // namespace astro 