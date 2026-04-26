#include "pow.h"
#include "../crypto/sha256.h"
#include <chrono>
#include <iostream>

uint64_t ProofOfWork::mine(const std::string& blockData,
                            uint32_t difficulty,
                            std::string& outHash) {
    std::string target(difficulty, '0');
    uint64_t nonce = 0;

    auto start = std::chrono::steady_clock::now();

    while (true) {
        outHash = SHA256::hash(SHA256::hash(blockData + std::to_string(nonce)));
        if (outHash.substr(0, difficulty) == target) break;
        nonce++;
    }

    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "\033[33m[PoW] " << nonce << " hashes in "
              << elapsed << "s ("
              << (uint64_t)(nonce / elapsed) << " H/s)\033[0m\n";
    return nonce;
}

bool ProofOfWork::verify(const std::string& blockData,
                          uint32_t difficulty,
                          uint64_t nonce,
                          const std::string& hash) {
    std::string computed = SHA256::hash(
        SHA256::hash(blockData + std::to_string(nonce)));
    return computed == hash && meetsTarget(hash, difficulty);
}

bool ProofOfWork::meetsTarget(const std::string& hash, uint32_t difficulty) {
    if (hash.size() < difficulty) return false;
    return hash.substr(0, difficulty) == std::string(difficulty, '0');
}

double ProofOfWork::estimateHashRate(uint32_t difficulty) {
    // Rough estimate: 16^difficulty hashes needed on average
    double expected = 1.0;
    for (uint32_t i = 0; i < difficulty; i++) expected *= 16.0;
    return expected; // expected hashes needed
}
