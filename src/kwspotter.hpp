#pragma once

#include "sherpa-onnx/c-api/c-api.h"
#include <vector>

namespace astro {

class KWSpotter {
public:
    KWSpotter(int sample_rate);
    ~KWSpotter();

    void detect(const std::vector<float>& samples);
private:
    int m_sample_rate;

    const SherpaOnnxKeywordSpotter *m_kws = nullptr;
    const SherpaOnnxOnlineStream *m_stream = nullptr;
};

}