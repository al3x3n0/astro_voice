#include "config.hpp"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace astro {

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

void Config::loadFromFile(const std::string& configPath) {
    configPath_ = configPath;
    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configPath);
    }
    file >> config_;
}

void Config::loadDefaults() const {
    // Set default values if config file is not loaded
    if (config_.is_null()) {
        // Get current working directory
        std::filesystem::path cwd = std::filesystem::current_path();
        std::string cwd_str = cwd.string();

        config_ = {
            {"models", {
                {"vad", {
                    {"path", cwd_str + "/silero_vad.onnx"},
                    {"description", "Voice Activity Detection model"}
                }},
                {"kws", {
                    {"path", cwd_str + "/sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01"},
                    {"description", "Keyword Spotting model"},
                    {"tokens_type", "bpe"},
                    {"default_threshold", 0.5},
                    {"default_boost", 1.0}
                }},
                {"asr_zh", {
                    {"path", cwd_str + "/sherpa-onnx-paraformer-zh-small-2024-03-09"},
                    {"description", "Chinese Speech Recognition model"}
                }},
                {"asr_en", {
                    {"path", cwd_str + "/sherpa-onnx-zipformer-gigaspeech-2023-12-12"},
                    {"description", "English Speech Recognition model"}
                }},
                {"asr_multi", {
                    {"path", cwd_str + "/sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17"},
                    {"description", "Multilingual Speech Recognition model"}
                }},
                {"tts", {
                    {"path", cwd_str + "/matcha-icefall-en_US-ljspeech"},
                    {"description", "Text-to-Speech model"},
                    {"vocoder", cwd_str + "/vocos-22khz-univ.onnx"},
                    {"tokens", cwd_str + "/matcha-icefall-en_US-ljspeech/tokens.txt"},
                    {"data_dir", cwd_str + "/matcha-icefall-en_US-ljspeech/espeak-ng-data"},
                    {"num_threads", 1},
                    {"debug", true}
                }}
            }},
            {"keywords", {
                {"default_threshold", 0.5},
                {"default_boost", 1.0},
                {"output_dir", cwd_str + "/keywords"}
            }},
            {"build_dir", cwd_str}
        };
    }
}

void Config::checkFile(const std::string& path, const std::string& description) const {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(description + " file not found: " + path);
    }
}

void Config::checkDirectory(const std::string& path, const std::string& description) const {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(description + " directory not found: " + path);
    }
}

void Config::checkFiles() const {
    loadDefaults();

    // Check VAD model
    checkFile(getVadModelPath(), "VAD model");

    // Check KWS model files
    std::string kws_path = getKwsModelPath();
    checkDirectory(kws_path, "KWS model directory");
    checkFile(kws_path + "/encoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx", "KWS encoder model");
    checkFile(kws_path + "/decoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx", "KWS decoder model");
    checkFile(kws_path + "/joiner-epoch-12-avg-2-chunk-16-left-64.int8.onnx", "KWS joiner model");
    checkFile(kws_path + "/tokens.txt", "KWS tokens file");

    // Check ASR model files
    std::string asr_en_path = getAsrEnModelPath();
    checkDirectory(asr_en_path, "English ASR model directory");
    checkFile(asr_en_path + "/encoder-epoch-30-avg-1.int8.onnx", "English ASR encoder model");
    checkFile(asr_en_path + "/decoder-epoch-30-avg-1.int8.onnx", "English ASR decoder model");
    checkFile(asr_en_path + "/joiner-epoch-30-avg-1.int8.onnx", "English ASR joiner model");
    checkFile(asr_en_path + "/tokens.txt", "English ASR tokens file");

    // Check TTS model files
    checkFile(getTtsModelPath() + "/model-steps-3.onnx", "TTS acoustic model");
    checkFile(getTtsVocoderPath(), "TTS vocoder model");
    checkFile(getTtsTokensPath(), "TTS tokens file");
    checkDirectory(getTtsDataDir(), "TTS data directory");

    // Check keywords directory
    checkDirectory(getKeywordsOutputDir(), "Keywords output directory");
}

std::string Config::getVadModelPath() const {
    loadDefaults();
    return config_["models"]["vad"]["path"];
}

std::string Config::getKwsModelPath() const {
    loadDefaults();
    return config_["models"]["kws"]["path"];
}

std::string Config::getAsrZhModelPath() const {
    loadDefaults();
    return config_["models"]["asr_zh"]["path"];
}

std::string Config::getAsrEnModelPath() const {
    loadDefaults();
    return config_["models"]["asr_en"]["path"];
}

std::string Config::getAsrMultiModelPath() const {
    loadDefaults();
    return config_["models"]["asr_multi"]["path"];
}

std::string Config::getKwsTokensType() const {
    loadDefaults();
    return config_["models"]["kws"]["tokens_type"];
}

double Config::getKwsDefaultThreshold() const {
    loadDefaults();
    return config_["models"]["kws"]["default_threshold"];
}

double Config::getKwsDefaultBoost() const {
    loadDefaults();
    return config_["models"]["kws"]["default_boost"];
}

double Config::getKeywordsDefaultThreshold() const {
    loadDefaults();
    return config_["keywords"]["default_threshold"];
}

double Config::getKeywordsDefaultBoost() const {
    loadDefaults();
    return config_["keywords"]["default_boost"];
}

std::string Config::getKeywordsOutputDir() const {
    loadDefaults();
    return config_["keywords"]["output_dir"];
}

std::string Config::getBuildDir() const {
    loadDefaults();
    return config_["build_dir"];
}

// TTS Getters
std::string Config::getTtsModelPath() const {
    loadDefaults();
    return config_["models"]["tts"]["path"];
}

std::string Config::getTtsVocoderPath() const {
    loadDefaults();
    return config_["models"]["tts"]["vocoder"];
}

std::string Config::getTtsTokensPath() const {
    loadDefaults();
    return config_["models"]["tts"]["tokens"];
}

std::string Config::getTtsDataDir() const {
    loadDefaults();
    return config_["models"]["tts"]["data_dir"];
}

int Config::getTtsNumThreads() const {
    loadDefaults();
    return config_["models"]["tts"]["num_threads"];
}

bool Config::getTtsDebug() const {
    loadDefaults();
    return config_["models"]["tts"]["debug"];
}

} // namespace astro 