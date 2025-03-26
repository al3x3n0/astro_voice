#include "astro_client.hpp"
#include "base64.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <stdexcept>


namespace astro {

using json = nlohmann::json;


AstroBackendClient::AstroBackendClient(const std::string& url) : base_url(url) {}

AstroResponse AstroBackendClient::send_askie_text(const std::string& text) {
    // Create JSON payload
    json payload;
    payload["script"] = text;

    // Send POST request
    cpr::Response response = cpr::Post(
        cpr::Url{base_url},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{payload.dump()}
    );

    // Check if request was successful
    if (response.status_code != 200) {
        throw std::runtime_error("HTTP request failed with status code: " + 
                               std::to_string(response.status_code));
    }

    // Parse response JSON
    try {
        json response_json = json::parse(response.text);
        
        // Check if response contains text field
        if (!response_json.contains("full_text")) {
            throw std::runtime_error("Response JSON does not contain 'text' field");
        }

        auto full_text = response_json["full_text"].get<std::string>();
        printf("full_text: %s\n", full_text.c_str());
        return {full_text, base64_decode(response_json["audio_base64"].get<std::string>())};
    } catch (const json::parse_error& e) {
        throw std::runtime_error("Failed to parse response JSON: " + std::string(e.what()));
    }
}

} // namespace astro
