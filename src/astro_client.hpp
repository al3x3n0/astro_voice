#pragma once

#include <string>

namespace astro {

class AstroBackendClient {
private:
    std::string base_url;

public:
    AstroBackendClient(const std::string& url);
    std::string send_askie_text(const std::string& text);
};

} // namespace astro
