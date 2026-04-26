#pragma once
#include "pow.h"
#include "pos.h"
#include <string>

// Hybrid Consensus — PoW proposes blocks, PoS finalizes them
// This is more secure than either alone (like Decred's model)
class HybridConsensus {
public:
    HybridConsensus();

    // Phase 1: Miner does PoW to propose a block
    bool proposeBlock(const std::string& blockData,
                      uint32_t difficulty,
                      uint64_t nonce,
                      const std::string& hash);

    // Phase 2: PoS validators vote to finalize
    bool finalizeBlock(const std::string& blockHash,
                       const std::vector<std::string>& voterAddresses);

    // Add a PoS validator
    void addValidator(const std::string& address, uint64_t stake);

    // Check if a block is fully confirmed (PoW + PoS)
    bool isConfirmed(const std::string& blockHash) const;

    // Get the PoS component
    ProofOfStake& getPoS() { return pos; }

    void printStatus() const;

private:
    ProofOfStake pos;

    // Votes needed to finalize (2/3 of validators)
    static constexpr double FINALITY_THRESHOLD = 0.67;

    std::unordered_map<std::string, int> voteCount;
    std::unordered_map<std::string, bool> finalized;
};
