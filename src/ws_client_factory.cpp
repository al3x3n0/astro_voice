#include "ws_client_factory.hpp"

namespace astro {

WSClientFactory::WSClientFactory(const std::string& url)
    : url_(url) {
}

std::shared_ptr<WSClient> WSClientFactory::create() {
    return std::make_shared<WSClient>(url_);
}

std::string WSClientFactory::get_url() const {
    return url_;
}

} // namespace astro