#include "tts.hpp"
#include "config.hpp"
#include <functional>

namespace astro {

using namespace std::placeholders;

TTS::TTS() {
    auto& config = Config::getInstance();
    
    SherpaOnnxOfflineTtsConfig tts_config;
    memset(&tts_config, 0, sizeof(tts_config));
    
    // Set model paths from config
    tts_config.model.matcha.acoustic_model = (config.getTtsModelPath() + "/model-steps-3.onnx").c_str();
    tts_config.model.matcha.vocoder = config.getTtsVocoderPath().c_str();
    tts_config.model.matcha.tokens = config.getTtsTokensPath().c_str();
    tts_config.model.matcha.data_dir = config.getTtsDataDir().c_str();

    // Set model parameters
    tts_config.model.num_threads = config.getTtsNumThreads();
    tts_config.model.debug = config.getTtsDebug();

    m_tts = SherpaOnnxCreateOfflineTts(&tts_config);
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
