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


namespace astro {

VoicePipeline::VoicePipeline(const std::string& astro_url,
        std::shared_ptr<AudioQueue> audio_queue,
        std::shared_ptr<VoiceActivityDetector> vad_,
        std::shared_ptr<KWSpotter> kws_,
        std::shared_ptr<ParaformerSTT> stt_,
        std::shared_ptr<AstroBackendClient> astro_client_) 
    : m_audio_queue(audio_queue)
    , vad(vad_)
    , kws(kws_)
    , stt(stt_)
    , astro_client(astro_client_) {
}

void VoicePipeline::start() {
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
                        //std::string response = astro_client->send_askie_text(text);
                        // Response handling can be added here
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

} // namespace astro