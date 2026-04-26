#pragma once
#include <string>
#include <cstdint>

// Proof of Work — SHA-256 based (identical to Bitcoin)
class ProofOfWork {
public:
    // Mine: find nonce such that hash starts with 'difficulty' zeros
    static uint64_t mine(const std::string& blockData,
                         uint32_t difficulty,
                         std::string& outHash);

    // Verify a mined result
    static bool verify(const std::string& blockData,
                       uint32_t difficulty,
                       uint64_t nonce,
                       const std::string& hash);

    // Check if hash meets difficulty target
    static bool meetsTarget(const std::string& hash, uint32_t difficulty);

    // Estimate hash rate (hashes per second)
    static double estimateHashRate(uint32_t difficulty);
};
