#include "base58.h"
#include "sha256.h"
#include <algorithm>
#include <stdexcept>

const std::string Base58::ALPHABET =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string Base58::encode(const std::vector<uint8_t>& data) {
    // Count leading zeros
    int zeros = 0;
    for (uint8_t b : data) { if (b==0) zeros++; else break; }

    // Convert to big integer via base256 → base58
    std::vector<uint8_t> tmp(data.begin(), data.end());
    std::string result;

    while (!tmp.empty()) {
        int rem = 0;
        std::vector<uint8_t> next;
        for (uint8_t b : tmp) {
            int cur = rem * 256 + b;
            if (!next.empty() || cur/58 > 0)
                next.push_back((uint8_t)(cur/58));
            rem = cur % 58;
        }
        result += ALPHABET[rem];
        tmp = next;
    }

    // Add '1' for each leading zero byte
    result += std::string(zeros, '1');
    std::reverse(result.begin(), result.end());
    return result;
}

std::string Base58::encodeCheck(uint8_t version,
                                  const std::vector<uint8_t>& payload) {
    // Build: version || payload
    std::vector<uint8_t> data;
    data.push_back(version);
    data.insert(data.end(), payload.begin(), payload.end());

    // Checksum = first 4 bytes of SHA256(SHA256(data))
    std::string dataStr(data.begin(), data.end());
    std::string h1 = SHA256::hash(dataStr);
    // Convert hex hash to bytes for double hash
    std::vector<uint8_t> h1bytes;
    for (size_t i=0; i<h1.size(); i+=2)
        h1bytes.push_back((uint8_t)std::stoi(h1.substr(i,2),nullptr,16));

    std::string h1str(h1bytes.begin(), h1bytes.end());
    std::string h2 = SHA256::hash(h1str);

    // Append 4 checksum bytes
    for (int i=0; i<8; i+=2)
        data.push_back((uint8_t)std::stoi(h2.substr(i,2),nullptr,16));

    return encode(data);
}

std::vector<uint8_t> Base58::decode(const std::string& encoded) {
    // Count leading '1's
    int zeros = 0;
    for (char c : encoded) { if (c=='1') zeros++; else break; }

    std::vector<uint8_t> result;
    for (char c : encoded) {
        size_t pos = ALPHABET.find(c);
        if (pos == std::string::npos) throw std::invalid_argument("Invalid Base58");

        int carry = (int)pos;
        for (auto it=result.rbegin(); it!=result.rend(); it++) {
            carry += 58 * (*it);
            *it = carry & 0xFF;
            carry >>= 8;
        }
        while (carry) { result.insert(result.begin(), carry&0xFF); carry>>=8; }
    }

    // Add leading zero bytes
    result.insert(result.begin(), zeros, 0x00);
    return result;
}

bool Base58::decodeCheck(const std::string& encoded,
                           uint8_t& version,
                           std::vector<uint8_t>& payload) {
    try {
        auto data = decode(encoded);
        if (data.size() < 5) return false;

        // Split: version(1) | payload(n) | checksum(4)
        version = data[0];
        payload = std::vector<uint8_t>(data.begin()+1, data.end()-4);
        std::vector<uint8_t> checksum(data.end()-4, data.end());

        // Recompute checksum
        std::vector<uint8_t> toHash(data.begin(), data.end()-4);
        std::string toHashStr(toHash.begin(), toHash.end());
        std::string h1 = SHA256::hash(toHashStr);
        std::vector<uint8_t> h1bytes;
        for (size_t i=0;i<h1.size();i+=2)
            h1bytes.push_back((uint8_t)std::stoi(h1.substr(i,2),nullptr,16));
        std::string h2 = SHA256::hash(std::string(h1bytes.begin(),h1bytes.end()));

        for (int i=0;i<4;i++)
            if (checksum[i] != (uint8_t)std::stoi(h2.substr(i*2,2),nullptr,16))
                return false;
        return true;
    } catch (...) { return false; }
}

bool Base58::isValid(const std::string& address) {
    uint8_t version; std::vector<uint8_t> payload;
    return decodeCheck(address, version, payload);
}
