#include "audio_stream.hpp"
#include "pipe.hpp"
#include "astro_client.hpp"
#include "vad.hpp"
#include "kwspotter.hpp"
#include "paraformer.hpp"
#include "audio_queue.hpp"


int main(int argc, char **argv) {

    auto audio_queue = std::make_shared<astro::AudioQueue>();
    astro::AudioStream as(audio_queue);

    astro::VoicePipeline pipeline("http://localhost:8080",
        audio_queue,
        std::make_shared<astro::VoiceActivityDetector>(),
        std::make_shared<astro::KWSpotter>(16000),
        std::make_shared<astro::ParaformerSTT>(),
        std::make_shared<astro::AstroBackendClient>("http://localhost:8080"));

    pipeline.start();
    as.start();

    return 0;
}
