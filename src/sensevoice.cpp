#include "sensevoice.hpp"

namespace astro {

SenseVoice::SenseVoice() {
    
    SherpaOnnxOfflineSenseVoiceModelConfig sense_voice_config;
    memset(&sense_voice_config, 0, sizeof(sense_voice_config));
    sense_voice_config.model = "./sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17/model.int8.onnx";
    sense_voice_config.language = "zh";
    sense_voice_config.use_itn = 1;

    // Offline model config
    SherpaOnnxOfflineModelConfig offline_model_config;
    memset(&offline_model_config, 0, sizeof(offline_model_config));
    offline_model_config.debug = 0;
    offline_model_config.num_threads = 1;
    offline_model_config.provider = "cpu";
    offline_model_config.tokens = "./sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17/tokens.txt";
    offline_model_config.sense_voice = sense_voice_config;

    // Recognizer config
    SherpaOnnxOfflineRecognizerConfig recognizer_config;
    memset(&recognizer_config, 0, sizeof(recognizer_config));
    recognizer_config.decoding_method = "greedy_search";
    recognizer_config.model_config = offline_model_config;

    m_recognizer = SherpaOnnxCreateOfflineRecognizer(&recognizer_config);
    
}

SenseVoice::~SenseVoice() {
    if (m_recognizer) {
        SherpaOnnxDestroyOfflineRecognizer(m_recognizer);
    }
}

void SenseVoice::detect(const std::vector<float>& samples) {
    auto stream = SherpaOnnxCreateOfflineStream(m_recognizer);
    SherpaOnnxAcceptWaveformOffline(stream, m_sample_rate, samples.data(), samples.size());
    SherpaOnnxDecodeOfflineStream(m_recognizer, stream);
    const SherpaOnnxOfflineRecognizerResult *result = SherpaOnnxGetOfflineStreamResult(stream);
    fprintf(stdout, "TEXT: %s\n", result->text);
    SherpaOnnxDestroyOfflineRecognizerResult(result);
    SherpaOnnxDestroyOfflineStream(stream);
}

}
