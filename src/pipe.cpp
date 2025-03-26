#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "vad.hpp"
#include "kwspotter.hpp"
#include "paraformer.hpp"
#include "astro_client.hpp"
#include "pipe.hpp"
#include "player.hpp"
#include "ws_client.hpp"

namespace astro {

VoicePipeline::VoicePipeline(
        std::shared_ptr<AudioQueue> audio_queue,
        std::shared_ptr<VoiceActivityDetector> vad_,
        std::shared_ptr<KWSpotter> kws_,
        std::shared_ptr<ParaformerSTT> stt_,
        std::shared_ptr<AstroBackendClient> astro_client_,
        std::shared_ptr<WSClient> ws_client_) 
    : m_audio_queue(audio_queue)
    , vad(vad_)
    , kws(kws_)
    , stt(stt_)
    , astro_client(astro_client_)
    , ws_client(ws_client_) {
    player = std::make_shared<AudioPlayer>();
}

void VoicePipeline::start() {
    ws_client->connect(
        [this](const std::string& text) {
            this->on_text(text);
        },
        [this](const std::vector<uint8_t>& audio) {
            this->on_audio(audio);
        }
    );
    m_processing_thread = std::thread(&VoicePipeline::run_processing_thread, this);
}

void VoicePipeline::stop() {
    m_should_stop = true;
    m_cv.notify_one();
    m_processing_thread.join();
}

void VoicePipeline::run_processing_thread() {
    while (true) {
        AudioData data;
        {
            m_audio_queue->wait();
            if (m_should_stop) {
                break;
            }

            if (!m_audio_queue->pop(data)) {
                continue;
            }
        }
        process_audio(data.samples);
    }
}

void VoicePipeline::process_audio(const std::vector<float>& audio_buffer) {
    static ProcessingState state;

    // Step 1: Voice Activity Detection
    std::vector<float> va_samples;
    bool has_voice = vad->detect(audio_buffer, va_samples);
    switch (state.state) {
        case PipelineState::WAITING_FOR_KEYWORD: {
            if (!has_voice) {
                return;
            }

            if (kws->detect(va_samples)) {
                printf("Keyword detected\n");
                state.state = PipelineState::TRANSCRIBING;
                state.transcription_buffer.insert(
                    state.transcription_buffer.end(),
                    va_samples.begin(),
                    va_samples.end()
                );
            }
            break;
        }

        case PipelineState::TRANSCRIBING: {
            if (has_voice) {
                state.transcription_buffer.insert(
                    state.transcription_buffer.end(),
                    va_samples.begin(),
                    va_samples.end()
                );
            } else {
                // Voice ended, process accumulated speech
                std::string text = stt->transcribe(state.transcription_buffer);
                if (!text.empty()) {
                    try {
                        printf("Sending text to astro: %s\n", text.c_str());
                        ws_client->send_message(text);
                        //auto response = astro_client->send_askie_text(text);
                        //printf("Astro response: %s\n", response.full_text.c_str());

                        //player->play(response.audio);
                        //printf("Playing audio\n");
                    } catch (const std::exception& e) {
                        // Handle errors appropriately
                    }
                }

                // Reset state
                state = ProcessingState();
            }
            break;
        }
    }
}

void VoicePipeline::on_text(const std::string& text) {
    printf("Received text from server: %s\n", text.c_str());
}

void VoicePipeline::on_audio(const std::vector<uint8_t>& audio) {
    printf("Received audio from server\n");
    try {        
        if (!audio.empty()) {
            player->play(audio);
            printf("Playing audio from websocket message\n");
        }
    } catch (const std::exception& e) {
        printf("Error processing websocket message: %s\n", e.what());
    }
}


} // namespace astro