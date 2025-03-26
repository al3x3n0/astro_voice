#pragma once

#include <vector>
#include <string>

namespace astro {

std::vector<uint8_t> base64_decode(const std::string& base64_str);

} // namespace astro
