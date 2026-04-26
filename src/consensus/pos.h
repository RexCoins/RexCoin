#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

struct Validator {
    std::string address;
    uint64_t    stake;        // Amount staked in satoshis
    uint64_t    rewards;      // Accumulated rewards
    bool        active;
};

// Proof of Stake — validators stake RXC to finalize blocks
class ProofOfStake {
public:
    // Register a validator with stake amount
    void addValidator(const std::string& address, uint64_t stakeAmount);

    // Remove / unstake
    void removeValidator(const std::string& address);

    // Select validator for next block (weighted by stake)
    std::string selectValidator(const std::string& seed) const;

    // Validate a block proposed by a validator
    bool validateBlock(const std::string& validatorAddress,
                       const std::string& blockHash) const;

    // Distribute staking rewards
    void distributeRewards(uint64_t totalReward);

    // Get total staked amount
    uint64_t totalStaked() const;

    // Get all validators
    std::vector<Validator> getValidators() const;

    void print() const;

private:
    std::unordered_map<std::string, Validator> validators;

    // Minimum stake to become a validator (1000 RXC)
    static constexpr uint64_t MIN_STAKE = 1000ULL * 100000000ULL;
};
