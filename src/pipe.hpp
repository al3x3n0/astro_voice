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
class TTS;
class AstroBackendClient;
class AudioPlayer;
class WSClientFactory;  
class WSClient;

enum class PipelineState {
    WAITING_FOR_KEYWORD,
    TRANSCRIBING,
    SPEAKING,
};

class VoicePipeline {
public:
    class ProcessingState {
    public:
        PipelineState state = PipelineState::WAITING_FOR_KEYWORD;
        std::vector<float> transcription_buffer;
    };

    VoicePipeline(std::shared_ptr<AudioQueue> audio_queue,
                    std::shared_ptr<VoiceActivityDetector> vad_,
                    std::shared_ptr<KWSpotter> kws_,
                    std::shared_ptr<STTBase> stt_,
                    std::shared_ptr<AstroBackendClient> astro_client_,
                    std::shared_ptr<WSClientFactory> ws_client_factory_);

    void start();
    void run_processing_thread();
    void stop();

    void on_text(const std::string& text);
    void on_audio(const std::vector<uint8_t>& audio);

private:
    void send_text(const std::string& text);
    void process_audio(const std::vector<float>& audio_buffer);

    std::shared_ptr<VoiceActivityDetector> vad;
    std::shared_ptr<KWSpotter> kws;
    std::shared_ptr<STTBase> stt;
    std::shared_ptr<AstroBackendClient> astro_client;

    std::shared_ptr<WSClient> ws_client = nullptr;
    std::shared_ptr<WSClientFactory> ws_client_factory;
    std::shared_ptr<AudioPlayer> player;
    std::shared_ptr<TTS> tts;

    std::shared_ptr<AudioQueue> m_audio_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_should_stop = false;

    std::thread m_processing_thread;
};

} // namespace astro
