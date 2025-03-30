#include "zipformer_stt.hpp"

namespace astro {

ZipformerSTT::ZipformerSTT() {
    // Zipformer config
    SherpaOnnxOfflineTransducerModelConfig zipformer_config;
    memset(&zipformer_config, 0, sizeof(zipformer_config));
    zipformer_config.encoder = "sherpa-onnx-zipformer-gigaspeech-2023-12-12/encoder-epoch-30-avg-1.int8.onnx";
    zipformer_config.decoder = "sherpa-onnx-zipformer-gigaspeech-2023-12-12/decoder-epoch-30-avg-1.int8.onnx";
    zipformer_config.joiner = "sherpa-onnx-zipformer-gigaspeech-2023-12-12/joiner-epoch-30-avg-1.int8.onnx";

    // Offline model config
    SherpaOnnxOfflineModelConfig offline_model_config;
    memset(&offline_model_config, 0, sizeof(offline_model_config));
    offline_model_config.debug = 1;
    offline_model_config.num_threads = 1;
    offline_model_config.provider = "cpu";
    offline_model_config.tokens = "sherpa-onnx-zipformer-gigaspeech-2023-12-12/tokens.txt";
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
