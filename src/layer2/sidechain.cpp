#include "sidechain.h"
#include "../crypto/sha256.h"
#include <iostream>

Peg Sidechain::lockFunds(const std::string& mainTxHash,
                           const std::string& sidechainAddr,
                           uint64_t amount) {
    Peg peg;
    peg.mainChainTxHash  = mainTxHash;
    peg.sidechainAddress = sidechainAddr;
    peg.amount           = amount;
    peg.claimed          = false;
    std::cout << "[Sidechain:" << name << "] Locked "
              << amount/1e8 << " RXC\n";
    return peg;
}

bool Sidechain::unlockFunds(const std::string&) {
    std::cout << "[Sidechain:" << name << "] Funds unlocked\n";
    return true;
}
