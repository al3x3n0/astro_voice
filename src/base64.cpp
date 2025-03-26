#include <vector>
#include <string>

namespace astro {

std::vector<uint8_t> base64_decode(const std::string& base64_str) {
    const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::vector<uint8_t> decoded;
    int val = 0;
    int valb = -8;
    
    for (char c : base64_str) {
        if (c == '=') {
            break;
        }
        
        if (c == ' ' || c == '\n' || c == '\r') {
            continue;
        }
        
        size_t pos = base64_chars.find(c);
        if (pos == std::string::npos) {
            // Invalid character found
            continue;
        }
        
        val = (val << 6) + static_cast<int>(pos);
        valb += 6;
        
        if (valb >= 0) {
            decoded.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    
    return decoded;
}

} // namespace astro
