#include "kwspotter.hpp"
#include <iostream>


namespace astro {

KWSpotter::KWSpotter(int sample_rate) :
    m_sample_rate (sample_rate)
{
    SherpaOnnxKeywordSpotterConfig config;
    memset(&config, 0, sizeof(config));
    config.model_config.transducer.encoder =
        "./sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/"
        //"./sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile/"
        "encoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx";

    config.model_config.transducer.decoder =
        "./sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/"
        //"./sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile/"
        "decoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx";

    config.model_config.transducer.joiner =
        "./sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/"
        //"./sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile/"
        "joiner-epoch-12-avg-2-chunk-16-left-64.int8.onnx";

    config.model_config.tokens =
        "./sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/"
        //"./sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile/"
        "tokens.txt";

    config.model_config.provider = "cpu";
    config.model_config.num_threads = 1;
    config.model_config.debug = 1;

    config.keywords_file =
        //"./sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/"
        //"./sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile/"
        //"test_wavs/test_keywords.txt";
        "output.txt";

    m_kws = SherpaOnnxCreateKeywordSpotter(&config);
    
    m_stream = SherpaOnnxCreateKeywordStreamWithKeywords(m_kws, "▁HE LL O ▁AS K");
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

}
 