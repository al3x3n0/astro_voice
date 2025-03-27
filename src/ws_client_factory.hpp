#pragma once

#include "ws_client.hpp"

namespace astro {

class WSClientFactory {
public:
    WSClientFactory(const std::string& url);
    std::shared_ptr<WSClient> create();
    std::string get_url() const;

private:
    std::string url_;
};

} // namespace astro    
