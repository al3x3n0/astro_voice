#pragma once

#include <vector>
#include <string>

namespace astro {

class STTBase {
public:
    virtual ~STTBase() = default;
    virtual std::string transcribe(const std::vector<float>& samples) = 0;

protected:
    int m_sample_rate = 16000;
};

} // namespace astro
