#pragma once

#include "sherpa-onnx/c-api/c-api.h"
#include <vector>

namespace astro {

class Paraformer {
public:
    Paraformer();
    ~Paraformer();

    void detect(const std::vector<float>& samples);
private:
    int m_sample_rate = 16000;
    const SherpaOnnxOfflineRecognizer *m_recognizer = nullptr;
};

}
