#include "vad.hpp"
#include <iostream>


namespace astro {

VoiceActivityDetector::VoiceActivityDetector()
{
    SherpaOnnxVadModelConfig vadConfig;
    memset(&vadConfig, 0, sizeof(vadConfig));

    vadConfig.silero_vad.model = "./silero_vad.onnx";
    vadConfig.silero_vad.threshold = 0.5;
    vadConfig.silero_vad.min_silence_duration = 0.5;
    vadConfig.silero_vad.min_speech_duration = 0.5;
    vadConfig.silero_vad.max_speech_duration = 5;
    vadConfig.silero_vad.window_size = 512;
    vadConfig.sample_rate = 16000;
    vadConfig.num_threads = 1;
    vadConfig.debug = 1;

    m_vad = SherpaOnnxCreateVoiceActivityDetector(&vadConfig, 30);
    m_window_size = vadConfig.silero_vad.window_size;
}

VoiceActivityDetector::~VoiceActivityDetector() {
    if (m_vad) {
        SherpaOnnxDestroyVoiceActivityDetector(m_vad);
    }
}

bool VoiceActivityDetector::detect(const std::vector<float>& samples, std::vector<float>& out) {
    // Process audio in window_size chunks
    size_t offset = 0;
    while (offset + m_window_size <= samples.size()) {
        SherpaOnnxVoiceActivityDetectorAcceptWaveform(m_vad, 
            samples.data() + offset, m_window_size);
        offset += m_window_size;
    }

    // Process any remaining samples
    if (offset < samples.size()) {
        SherpaOnnxVoiceActivityDetectorAcceptWaveform(m_vad,
            samples.data() + offset, samples.size() - offset);
    }

    while (!SherpaOnnxVoiceActivityDetectorEmpty(m_vad)) {
        const SherpaOnnxSpeechSegment *segment = SherpaOnnxVoiceActivityDetectorFront(m_vad);
        
        float start = segment->start / 16000.0f;
        float duration = segment->n / 16000.0f;
        float stop = start + duration;

        fprintf(stderr, "VAD: %.3f -- %.3f\n", start, stop);

        out = std::vector<float>(segment->samples, segment->samples + segment->n);

        SherpaOnnxDestroySpeechSegment(segment);
        SherpaOnnxVoiceActivityDetectorPop(m_vad);

        return true;
    }

    return false;
}

}
