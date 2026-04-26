#include "hybrid.h"
#include <iostream>
#include <unordered_map>

HybridConsensus::HybridConsensus() {}

bool HybridConsensus::proposeBlock(const std::string& blockData,
                                    uint32_t difficulty,
                                    uint64_t nonce,
                                    const std::string& hash) {
    bool ok = ProofOfWork::verify(blockData, difficulty, nonce, hash);
    if (ok)
        std::cout << "\033[32m[Hybrid] Block proposed via PoW ✓\033[0m\n";
    else
        std::cout << "\033[31m[Hybrid] PoW verification failed ✗\033[0m\n";
    return ok;
}

bool HybridConsensus::finalizeBlock(const std::string& blockHash,
                                     const std::vector<std::string>& voters) {
    auto validators = pos.getValidators();
    int totalActive = 0;
    for (const auto& v : validators) if (v.active) totalActive++;
    if (totalActive == 0) {
        // No validators yet — auto-finalize (early network)
        finalized[blockHash] = true;
        return true;
    }

    int votes = 0;
    for (const auto& voter : voters)
        if (pos.validateBlock(voter, blockHash)) votes++;

    voteCount[blockHash] = votes;
    double ratio = (double)votes / totalActive;

    if (ratio >= FINALITY_THRESHOLD) {
        finalized[blockHash] = true;
        std::cout << "\033[32m[Hybrid] Block finalized by PoS ("
                  << votes << "/" << totalActive << " votes) ✓\033[0m\n";
        return true;
    }
    std::cout << "\033[31m[Hybrid] Not enough votes ("
              << votes << "/" << totalActive << ")\033[0m\n";
    return false;
}

void HybridConsensus::addValidator(const std::string& address, uint64_t stake) {
    pos.addValidator(address, stake);
}

bool HybridConsensus::isConfirmed(const std::string& blockHash) const {
    auto it = finalized.find(blockHash);
    return it != finalized.end() && it->second;
}

void HybridConsensus::printStatus() const {
    std::cout << "\n=== Hybrid Consensus Status ===\n";
    std::cout << "  Finalized blocks: " << finalized.size() << "\n";
    pos.print();
}
