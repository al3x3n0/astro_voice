#include "zipformer_stt.hpp"
#include "config.hpp"

namespace astro {

ZipformerSTT::ZipformerSTT() {
    auto& config = Config::getInstance();
    std::string model_path = config.getAsrEnModelPath();
    
    // Zipformer config
    SherpaOnnxOfflineTransducerModelConfig zipformer_config;
    memset(&zipformer_config, 0, sizeof(zipformer_config));
    zipformer_config.encoder = (model_path + "/encoder-epoch-30-avg-1.int8.onnx").c_str();
    zipformer_config.decoder = (model_path + "/decoder-epoch-30-avg-1.int8.onnx").c_str();
    zipformer_config.joiner = (model_path + "/joiner-epoch-30-avg-1.int8.onnx").c_str();

    // Offline model config
    SherpaOnnxOfflineModelConfig offline_model_config;
    memset(&offline_model_config, 0, sizeof(offline_model_config));
    offline_model_config.debug = 1;
    offline_model_config.num_threads = 1;
    offline_model_config.provider = "cpu";
    offline_model_config.tokens = (model_path + "/tokens.txt").c_str();
    offline_model_config.transducer = zipformer_config;

    // Recognizer config
    SherpaOnnxOfflineRecognizerConfig recognizer_config;
    memset(&recognizer_config, 0, sizeof(recognizer_config));
    recognizer_config.decoding_method = "greedy_search";
    recognizer_config.model_config = offline_model_config;
    
    m_recognizer = SherpaOnnxCreateOfflineRecognizer(&recognizer_config);
}

ZipformerSTT::~ZipformerSTT() {
    if (m_recognizer) {
        SherpaOnnxDestroyOfflineRecognizer(m_recognizer);
    }
}

std::string ZipformerSTT::transcribe(const std::vector<float>& samples) {
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
