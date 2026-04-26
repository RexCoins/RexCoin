#include "dns_seed.h"
#include <iostream>

const std::vector<SeedNode> DNSSeed::SEEDS = {
    {"seed1.rexnetwork.io",  8333, "RexCoin Primary Seed"},
    {"seed2.rexnetwork.io",  8333, "RexCoin Secondary Seed"},
    {"seed3.rexnetwork.io",  8333, "RexCoin Tertiary Seed"},
    {"testnet.rexnetwork.io",18333,"RexCoin Testnet Seed"},
};

std::vector<SeedNode> DNSSeed::getSeeds() {
    return SEEDS;
}

std::vector<std::string> DNSSeed::resolve(const std::string& hostname) {
    // Real impl: getaddrinfo(hostname, ...)
    // Returns hardcoded fallback IPs for now
    std::cout << "[DNS] Resolving: " << hostname << "\n";
    return {"127.0.0.1"}; // localhost for testing
}

bool DNSSeed::isReachable(const std::string& ip, uint16_t port) {
    // Real impl: TCP connect attempt with timeout
    std::cout << "[DNS] Checking " << ip << ":" << port << "\n";
    return false; // No real network yet
}
