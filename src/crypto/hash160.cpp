#include "hash160.h"
#include "sha256.h"
#include "ripemd160.h"
#include "base58.h"
#include <sstream>
#include <iomanip>

std::string Hash160::hash(const std::string& input) {
    // Step 1: SHA256
    std::string sha = SHA256::hash(input);
    // Convert hex → bytes
    std::vector<uint8_t> shaBytes;
    for (size_t i=0; i<sha.size(); i+=2)
        shaBytes.push_back((uint8_t)std::stoi(sha.substr(i,2),nullptr,16));

    // Step 2: RIPEMD160
    auto result = RIPEMD160::hashBytes(shaBytes);

    std::stringstream ss;
    for (uint8_t b : result)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return ss.str();
}

std::vector<uint8_t> Hash160::hashBytes(const std::vector<uint8_t>& input) {
    std::string inputStr(input.begin(), input.end());
    std::string sha = SHA256::hash(inputStr);

    std::vector<uint8_t> shaBytes;
    for (size_t i=0; i<sha.size(); i+=2)
        shaBytes.push_back((uint8_t)std::stoi(sha.substr(i,2),nullptr,16));

    return RIPEMD160::hashBytes(shaBytes);
}

std::string Hash160::toAddress(const std::vector<uint8_t>& pubKeyBytes,
                                 uint8_t version) {
    // HASH160 of public key
    auto h160 = hashBytes(pubKeyBytes);
    // Base58Check encode with version byte
    return Base58::encodeCheck(version, h160);
}
