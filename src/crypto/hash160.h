#pragma once
#include <string>
#include <vector>
#include <cstdint>

// HASH160 = RIPEMD160(SHA256(input))
// This is how Bitcoin derives addresses from public keys
// Also used in P2PKH and P2SH scripts

class Hash160 {
public:
    // Returns 20-byte hash as hex string
    static std::string hash(const std::string& input);

    // Returns raw 20 bytes
    static std::vector<uint8_t> hashBytes(const std::vector<uint8_t>& input);

    // Full Bitcoin-style address from public key bytes
    // version: 0x00 = mainnet, 0x6F = testnet, 0x3C = RexCoin (R)
    static std::string toAddress(const std::vector<uint8_t>& pubKeyBytes,
                                  uint8_t version = 0x3C);
};
