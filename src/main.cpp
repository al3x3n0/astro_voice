#include "audio_stream.hpp"
#include "pipe.hpp"
#include "astro_client.hpp"
#include "vad.hpp"
#include "kwspotter.hpp"
#include "paraformer.hpp"
#include "audio_queue.hpp"
#include "ws_client.hpp"


int main(int argc, char **argv) {

    auto audio_queue = std::make_shared<astro::AudioQueue>();
    astro::AudioStream as(audio_queue);

    const std::string astro_url = "https://llm-server-8kx0.onrender.com/stream_script";
    const std::string ws_url = "wss://llm-server-8kx0.onrender.com/ws_stream_script";

    auto ws_client = std::make_shared<astro::WSClient>(ws_url);

    astro::VoicePipeline pipeline(
        audio_queue,
        std::make_shared<astro::VoiceActivityDetector>(),
        std::make_shared<astro::KWSpotter>(16000),
        std::make_shared<astro::ParaformerSTT>(),
        std::make_shared<astro::AstroBackendClient>(astro_url),
        ws_client);

    pipeline.start();
    as.start();

    return 0;
}
