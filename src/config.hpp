#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <stdexcept>

namespace astro {

class Config {
public:
    static Config& getInstance();
    
    void loadFromFile(const std::string& configPath);
    void loadDefaults() const;
    void checkFiles() const;

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

    void checkFile(const std::string& path, const std::string& description) const;
    void checkDirectory(const std::string& path, const std::string& description) const;

    std::string configPath_;
    mutable nlohmann::json config_;
};

} // namespace astro 