#include "audio_stream.hpp"
#include "pipe.hpp"
#include "astro_client.hpp"
#include "vad.hpp"
#include "kwspotter.hpp"
#include "paraformer.hpp"
#include "zipformer_stt.hpp"
#include "audio_queue.hpp"
#include "ws_client.hpp"
#include "ws_client_factory.hpp"

int main(int argc, char **argv) {

    auto audio_queue = std::make_shared<astro::AudioQueue>();
    astro::AudioStream as(audio_queue);

    const std::string astro_url = "https://llm-server-8kx0.onrender.com/stream_script";
    const std::string ws_url = "wss://llm-server-8kx0.onrender.com/ws_stream_script";

    auto ws_client_factory = std::make_shared<astro::WSClientFactory>(ws_url);

    astro::VoicePipeline pipeline(
        audio_queue,
        std::make_shared<astro::VoiceActivityDetector>(),
        std::make_shared<astro::KWSpotter>(16000),
        std::make_shared<astro::ZipformerSTT>(),
        std::make_shared<astro::AstroBackendClient>(astro_url),
        ws_client_factory);

    pipeline.start();
    as.start();

    return 0;
}
