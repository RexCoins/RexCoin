#pragma once
#include <string>
#include <cstdint>
#include <vector>

// RIPEMD-160 — Bitcoin uses HASH160 = RIPEMD160(SHA256(data))
// This is how Bitcoin addresses are derived
class RIPEMD160 {
public:
    static std::string hash(const std::string& input);
    static std::vector<uint8_t> hashBytes(const std::vector<uint8_t>& input);

private:
    static const uint32_t K1[5], K2[5];
    static const int R1[80], R2[80], S1[80], S2[80];

    static uint32_t f(int j, uint32_t x, uint32_t y, uint32_t z);
    static uint32_t rol(uint32_t x, int n);
    static std::string toHex(const uint8_t* data, size_t len);
};
