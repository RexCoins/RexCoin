#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Base58Check — Bitcoin's address encoding
// Avoids confusing characters: 0, O, I, l
// Includes 4-byte checksum at end

class Base58 {
public:
    static const std::string ALPHABET;

    // Encode bytes to Base58Check string
    static std::string encode(const std::vector<uint8_t>& data);

    // Encode with version byte + 4-byte checksum (Bitcoin-style)
    static std::string encodeCheck(uint8_t version,
                                    const std::vector<uint8_t>& payload);

    // Decode Base58Check, verify checksum
    static bool decodeCheck(const std::string& encoded,
                             uint8_t& version,
                             std::vector<uint8_t>& payload);

    // Raw decode (no checksum)
    static std::vector<uint8_t> decode(const std::string& encoded);

    // Validate a Base58Check address
    static bool isValid(const std::string& address);
};
