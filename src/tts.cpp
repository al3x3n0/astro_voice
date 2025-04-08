#include "tts.hpp"
#include "config.hpp"
#include <functional>

namespace astro {

using namespace std::placeholders;

TTS::TTS() {
    auto& config = Config::getInstance();
    
    // Store configuration paths
    m_acoustic_model_path = config.getTtsModelPath() + "/model-steps-3.onnx";
    m_vocoder_path = config.getTtsVocoderPath();
    m_tokens_path = config.getTtsTokensPath();
    m_data_dir = config.getTtsDataDir();
    
    SherpaOnnxOfflineTtsConfig tts_config;
    memset(&tts_config, 0, sizeof(tts_config));
    
    // Set model paths from member variables
    tts_config.model.matcha.acoustic_model = m_acoustic_model_path.c_str();
    tts_config.model.matcha.vocoder = m_vocoder_path.c_str();
    tts_config.model.matcha.tokens = m_tokens_path.c_str();
    tts_config.model.matcha.data_dir = m_data_dir.c_str();

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
