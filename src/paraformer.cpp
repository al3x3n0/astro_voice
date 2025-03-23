#include "paraformer.hpp"

namespace astro {

ParaformerSTT::ParaformerSTT() {
    

    // Paraformer config
    SherpaOnnxOfflineParaformerModelConfig paraformer_config;
    memset(&paraformer_config, 0, sizeof(paraformer_config));
    paraformer_config.model = "sherpa-onnx-paraformer-zh-small-2024-03-09/model.int8.onnx";

    // Offline model config
    SherpaOnnxOfflineModelConfig offline_model_config;
    memset(&offline_model_config, 0, sizeof(offline_model_config));
    offline_model_config.debug = 1;
    offline_model_config.num_threads = 1;
    offline_model_config.provider = "cpu";
    offline_model_config.tokens = "sherpa-onnx-paraformer-zh-small-2024-03-09/tokens.txt";
    offline_model_config.paraformer = paraformer_config;

    // Recognizer config
    SherpaOnnxOfflineRecognizerConfig recognizer_config;
    memset(&recognizer_config, 0, sizeof(recognizer_config));
    recognizer_config.decoding_method = "greedy_search";
    recognizer_config.model_config = offline_model_config;
    
    m_recognizer = SherpaOnnxCreateOfflineRecognizer(&recognizer_config);
}

ParaformerSTT::~ParaformerSTT() {
    if (m_recognizer) {
        SherpaOnnxDestroyOfflineRecognizer(m_recognizer);
    }
}

std::string ParaformerSTT::transcribe(const std::vector<float>& samples) {
    auto stream = SherpaOnnxCreateOfflineStream(m_recognizer);
    SherpaOnnxAcceptWaveformOffline(stream, m_sample_rate, samples.data(), samples.size());
    SherpaOnnxDecodeOfflineStream(m_recognizer, stream);
    const SherpaOnnxOfflineRecognizerResult *result = SherpaOnnxGetOfflineStreamResult(stream);

    std::string text = result->text;
    fprintf(stdout, "TEXT: %s\n", result->text);
 
    SherpaOnnxDestroyOfflineRecognizerResult(result);
    SherpaOnnxDestroyOfflineStream(stream);

    return text;
}

} // namespace astro    
