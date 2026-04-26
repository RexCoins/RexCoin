#pragma once
#include <string>
#include <cstdint>

// SHA3-256 (Keccak) — Used for advanced hashing in RexCoin
class SHA3 {
public:
    static std::string hash256(const std::string& input);
    static std::string hash512(const std::string& input);

private:
    static const uint64_t RC[24];        // Round constants
    static const int      ROT[5][5];     // Rotation offsets

    static void keccakF(uint64_t state[25]);
    static std::string toHex(const uint8_t* data, size_t len);
};
