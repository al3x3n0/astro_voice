#include <cxxopts.hpp>
#include <iostream>
#include <filesystem>

#include "audio_stream.hpp"
#include "pipe.hpp"
#include "astro_client.hpp"
#include "vad.hpp"
#include "kwspotter.hpp"
#include "paraformer.hpp"
#include "zipformer_stt.hpp"
#include "audio_queue.hpp"
#include "ws_client.hpp"
#include "ws_client_factory.hpp"
#include "config.hpp"

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        cxxopts::Options options("astro", "Astro voice assistant");
        options.add_options()
            ("c,config", "Path to config file", cxxopts::value<std::string>())
            ("h,help", "Print usage");

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        // Load configuration
        auto& config = astro::Config::getInstance();
        
        if (result.count("config")) {
            std::string configPath = result["config"].as<std::string>();
            if (!std::filesystem::exists(configPath)) {
                throw std::runtime_error("Config file not found: " + configPath);
            }
            config.loadFromFile(configPath);
        } else {
            std::cerr << "Warning: No config file specified. Using default configuration." << std::endl;
        }

        // Print configuration
        std::cout << "Configuration loaded:" << std::endl;
        std::cout << "VAD Model: " << config.getVadModelPath() << std::endl;
        std::cout << "KWS Model: " << config.getKwsModelPath() << std::endl;
        std::cout << "ASR ZH Model: " << config.getAsrZhModelPath() << std::endl;
        std::cout << "ASR EN Model: " << config.getAsrEnModelPath() << std::endl;
        std::cout << "ASR Multi Model: " << config.getAsrMultiModelPath() << std::endl;
        std::cout << "Keywords Output Dir: " << config.getKeywordsOutputDir() << std::endl;

        // TODO: Initialize and use models with paths from config

        auto audio_queue = std::make_shared<astro::AudioQueue>();
        astro::AudioStream as(audio_queue);

        const std::string astro_url = "https://llm-server-8kx0.onrender.com/stream_script";
        const std::string ws_url = "wss://llm-server-8kx0.onrender.com/ws_stream_script";

        auto ws_client_factory = std::make_shared<astro::WSClientFactory>(ws_url);

        astro::VoicePipeline pipeline(
            audio_queue,
            std::make_shared<astro::VoiceActivityDetector>(),
            std::make_shared<astro::KWSpotter>(16000),
            std::make_shared<astro::ZipformerSTT>(),
            std::make_shared<astro::AstroBackendClient>(astro_url),
            ws_client_factory);

        pipeline.start();
        as.start();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
