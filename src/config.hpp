#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace astro {

class Config {
public:
    static Config& getInstance();
    
    void loadFromFile(const std::string& configPath);
    void loadDefaults() const;

    // Getters
    std::string getVadModelPath() const;
    std::string getKwsModelPath() const;
    std::string getAsrZhModelPath() const;
    std::string getAsrEnModelPath() const;
    std::string getAsrMultiModelPath() const;
    std::string getKwsTokensType() const;
    double getKwsDefaultThreshold() const;
    double getKwsDefaultBoost() const;
    double getKeywordsDefaultThreshold() const;
    double getKeywordsDefaultBoost() const;
    std::string getKeywordsOutputDir() const;
    std::string getBuildDir() const;

    // TTS Getters
    std::string getTtsModelPath() const;
    std::string getTtsVocoderPath() const;
    std::string getTtsTokensPath() const;
    std::string getTtsDataDir() const;
    int getTtsNumThreads() const;
    bool getTtsDebug() const;

private:
    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::string configPath_;
    mutable nlohmann::json config_;
};

} // namespace astro 