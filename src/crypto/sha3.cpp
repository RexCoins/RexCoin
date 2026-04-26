#include "sha3.h"
#include <cstring>
#include <sstream>
#include <iomanip>

const uint64_t SHA3::RC[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL,
    0x800000000000808AULL, 0x8000000080008000ULL,
    0x000000000000808BULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL,
    0x000000000000008AULL, 0x0000000000000088ULL,
    0x0000000080008009ULL, 0x000000008000000AULL,
    0x000000008000808BULL, 0x800000000000008BULL,
    0x8000000000008089ULL, 0x8000000000008003ULL,
    0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800AULL, 0x800000008000000AULL,
    0x8000000080008081ULL, 0x8000000000008080ULL,
    0x0000000080000001ULL, 0x8000000080008008ULL
};

const int SHA3::ROT[5][5] = {
    { 0, 36,  3, 41, 18},
    { 1, 44, 10, 45,  2},
    {62,  6, 43, 15, 61},
    {28, 55, 25, 21, 56},
    {27, 20, 39,  8, 14}
};

static inline uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

void SHA3::keccakF(uint64_t A[25]) {
    for (int round = 0; round < 24; round++) {
        uint64_t C[5], D[5], B[25];
        for (int x = 0; x < 5; x++)
            C[x] = A[x]^A[x+5]^A[x+10]^A[x+15]^A[x+20];
        for (int x = 0; x < 5; x++)
            D[x] = C[(x+4)%5] ^ rotl64(C[(x+1)%5], 1);
        for (int x = 0; x < 5; x++)
            for (int y = 0; y < 5; y++)
                A[x+5*y] ^= D[x];
        for (int x = 0; x < 5; x++)
            for (int y = 0; y < 5; y++)
                B[y+5*((2*x+3*y)%5)] = rotl64(A[x+5*y], ROT[x][y]);
        for (int x = 0; x < 5; x++)
            for (int y = 0; y < 5; y++)
                A[x+5*y] = B[x+5*y]^((~B[(x+1)%5+5*y])&B[(x+2)%5+5*y]);
        A[0] ^= RC[round];
    }
}

std::string SHA3::toHex(const uint8_t* data, size_t len) {
    std::stringstream ss;
    for (size_t i = 0; i < len; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

std::string SHA3::hash256(const std::string& input) {
    const size_t rate = 136; // SHA3-256 rate in bytes
    const size_t outputLen = 32;

    uint64_t state[25] = {};
    std::vector<uint8_t> msg(input.begin(), input.end());

    // Padding
    msg.push_back(0x06);
    while (msg.size() % rate != 0) msg.push_back(0x00);
    msg.back() |= 0x80;

    // Absorb
    for (size_t i = 0; i < msg.size(); i += rate) {
        for (size_t j = 0; j < rate/8; j++) {
            uint64_t lane = 0;
            for (int k = 0; k < 8; k++)
                lane |= (uint64_t)msg[i + j*8 + k] << (8*k);
            state[j] ^= lane;
        }
        keccakF(state);
    }

    // Squeeze
    uint8_t output[32];
    memcpy(output, state, outputLen);
    return toHex(output, outputLen);
}

std::string SHA3::hash512(const std::string& input) {
    // Simplified — returns double SHA3-256 for now
    return hash256(hash256(input));
}
