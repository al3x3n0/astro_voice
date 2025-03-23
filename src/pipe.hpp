#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "audio_queue.hpp"

namespace astro {

class VoiceActivityDetector;
class KWSpotter;
class ParaformerSTT;
class AstroBackendClient;

enum class PipelineState {
    WAITING_FOR_KEYWORD,
    TRANSCRIBING,
};

class VoicePipeline {
public:
    class ProcessingState {
    public:
        PipelineState state = PipelineState::WAITING_FOR_KEYWORD;
        std::vector<float> transcription_buffer;
    };

    VoicePipeline(const std::string& astro_url,
                    std::shared_ptr<AudioQueue> audio_queue,
                    std::shared_ptr<VoiceActivityDetector> vad_,
                    std::shared_ptr<KWSpotter> kws_,
                    std::shared_ptr<ParaformerSTT> stt_,
                    std::shared_ptr<AstroBackendClient> astro_client_);

    void start();
    void run_processing_thread();
    void stop();

private:
    void process_audio(const std::vector<float>& audio_buffer);

    std::shared_ptr<VoiceActivityDetector> vad;
    std::shared_ptr<KWSpotter> kws;
    std::shared_ptr<ParaformerSTT> stt;
    std::shared_ptr<AstroBackendClient> astro_client;

    std::shared_ptr<AudioQueue> m_audio_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_should_stop = false;

    std::thread m_processing_thread;
};

} // namespace astro
