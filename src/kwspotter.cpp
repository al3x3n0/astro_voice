#include "kwspotter.hpp"
#include "config.hpp"
#include <iostream>

namespace astro {

KWSpotter::KWSpotter(int sample_rate) :
    m_sample_rate (sample_rate)
{
    auto& config = Config::getInstance();
    std::string model_path = config.getKwsModelPath();
    
    // Store configuration paths
    m_encoder_path = model_path + "/encoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx";
    m_decoder_path = model_path + "/decoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx";
    m_joiner_path = model_path + "/joiner-epoch-12-avg-2-chunk-16-left-64.int8.onnx";
    m_tokens_path = model_path + "/tokens.txt";
    m_keywords_file = config.getKeywordsOutputDir() + "/output.txt";
    
    SherpaOnnxKeywordSpotterConfig kws_config;
    memset(&kws_config, 0, sizeof(kws_config));
    
    // Set model paths from member variables
    kws_config.model_config.transducer.encoder = m_encoder_path.c_str();
    kws_config.model_config.transducer.decoder = m_decoder_path.c_str();
    kws_config.model_config.transducer.joiner = m_joiner_path.c_str();
    kws_config.model_config.tokens = m_tokens_path.c_str();

    // Set model parameters
    kws_config.model_config.provider = "cpu";
    kws_config.model_config.num_threads = 1;
    kws_config.model_config.debug = 1;

    // Set keywords file path
    kws_config.keywords_file = m_keywords_file.c_str();
    printf("Keywords file: %s\n", kws_config.keywords_file);
    
    // Set keywords
    m_kws = SherpaOnnxCreateKeywordSpotter(&kws_config);
    
    // Use default threshold and boost from config
    m_stream = SherpaOnnxCreateKeywordStreamWithKeywords(
        m_kws, 
        "▁HE LL O ▁AS K");
}
  
KWSpotter::~KWSpotter() {
    if (m_stream) {
        SherpaOnnxDestroyOnlineStream(m_stream);
    }
}

bool KWSpotter::detect(const std::vector<float>& samples) {
    SherpaOnnxOnlineStreamAcceptWaveform(m_stream, m_sample_rate, samples.data(), samples.size());
    while (SherpaOnnxIsKeywordStreamReady(m_kws, m_stream)) {
        SherpaOnnxDecodeKeywordStream(m_kws, m_stream);
        printf("Decoded keyword stream\n");
        fflush(stdout);
        const SherpaOnnxKeywordResult *r = SherpaOnnxGetKeywordResult(m_kws, m_stream);
        if (r && r->json && strlen(r->keyword)) {
            fprintf(stderr, "Detected keyword: %s\n", r->json);
            // Remember to reset the keyword stream
            SherpaOnnxResetKeywordStream(m_kws, m_stream);
            return true;
        }
        SherpaOnnxDestroyKeywordResult(r);
    }
    return false;
}

} // namespace astro
 