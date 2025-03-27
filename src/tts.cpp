#include "tts.hpp"
#include <functional>


namespace astro {

using namespace std::placeholders;

TTS::TTS() {
    SherpaOnnxOfflineTtsConfig config;
    memset(&config, 0, sizeof(config));
    config.model.matcha.acoustic_model =
        "./matcha-icefall-zh-baker/model-steps-3.onnx";
    config.model.matcha.vocoder = "./vocos-22khz-univ.onnx";
    config.model.matcha.lexicon = "./matcha-icefall-zh-baker/lexicon.txt";
    config.model.matcha.tokens = "./matcha-icefall-zh-baker/tokens.txt";
    config.model.matcha.dict_dir = "./matcha-icefall-zh-baker/dict";
    config.model.num_threads = 1;

    // If you don't want to see debug messages, please set it to 0
    config.model.debug = 1;

    // clang-format off
    config.rule_fsts = "./matcha-icefall-zh-baker/phone.fst,./matcha-icefall-zh-baker/date.fst,./matcha-icefall-zh-baker/number.fst";
    // clang-format on

    m_tts = SherpaOnnxCreateOfflineTts(&config);
}

TTS::~TTS() {
    SherpaOnnxDestroyOfflineTts(m_tts);
}

AudioData TTS::speak(const std::string& text) {
    const SherpaOnnxGeneratedAudio *audio = SherpaOnnxOfflineTtsGenerate(m_tts, text.c_str(), 0, 1.0);
    AudioData data;
    data.sample_rate = audio->sample_rate;
    data.samples.resize(audio->n);

    for (int i = 0; i < audio->n; i++) {  
        data.samples[i] = audio->samples[i];
    }

    SherpaOnnxDestroyOfflineTtsGeneratedAudio(audio);
    return data;
}

} // namespace astro
