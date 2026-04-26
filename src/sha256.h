#pragma once
#include <string>
#include <cstdint>
#include <vector>

class SHA256 {
public:
    static std::string hash(const std::string& input);

private:
    static const uint32_t K[64];
    static const uint32_t H0[8];
    static uint32_t rotr(uint32_t x, uint32_t n);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t sigma0(uint32_t x);
    static uint32_t sigma1(uint32_t x);
    static uint32_t gamma0(uint32_t x);
    static uint32_t gamma1(uint32_t x);
    static std::vector<uint8_t> pad(const std::string& input);
    static std::string toHex(const uint32_t hash[8]);
};
