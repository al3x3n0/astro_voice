#pragma once

#include <string>
#include <vector>

namespace astro {

class AstroResponse {
public:
    std::string full_text;
    std::vector<uint8_t> audio;
};

class AstroBackendClient {
private:
    std::string base_url;

public:
    AstroBackendClient(const std::string& url);
    AstroResponse send_askie_text(const std::string& text);
};

} // namespace astro
