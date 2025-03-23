#pragma once

#include "ts_queue.hpp"

namespace astro {

class AudioData {
public:
    std::vector<float> samples;
    int sample_rate;
};

typedef ThreadSafeQueue<AudioData> AudioQueue;

} // namespace astro
