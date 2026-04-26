#pragma once
#include <string>
#include <cstdint>

// Sidechain — parallel chain pegged to RexCoin main chain
// Allows specialized chains (e.g., fast chain, private chain)
struct Peg {
    std::string mainChainTxHash;
    std::string sidechainAddress;
    uint64_t    amount;
    bool        claimed;
};

class Sidechain {
public:
    std::string name;
    // Lock RXC on mainchain, receive equivalent on sidechain
    Peg lockFunds(const std::string& mainTxHash,
                   const std::string& sidechainAddr,
                   uint64_t amount);
    // Burn sidechain coins, unlock on mainchain
    bool unlockFunds(const std::string& pegId);
};
