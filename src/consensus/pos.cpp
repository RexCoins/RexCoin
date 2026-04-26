#include "pos.h"
#include "../crypto/sha256.h"
#include <iostream>
#include <stdexcept>
#include <numeric>

void ProofOfStake::addValidator(const std::string& address, uint64_t stakeAmount) {
    if (stakeAmount < MIN_STAKE)
        throw std::runtime_error("Stake too low. Minimum: 1000 RXC");

    Validator v;
    v.address = address;
    v.stake   = stakeAmount;
    v.rewards = 0;
    v.active  = true;
    validators[address] = v;

    std::cout << "\033[32m[PoS] Validator added: "
              << address.substr(0,20) << " stake="
              << stakeAmount / 100000000.0 << " RXC\033[0m\n";
}

void ProofOfStake::removeValidator(const std::string& address) {
    auto it = validators.find(address);
    if (it != validators.end()) it->second.active = false;
}

std::string ProofOfStake::selectValidator(const std::string& seed) const {
    if (validators.empty()) return "";

    // Weighted selection: higher stake = higher probability
    uint64_t total = totalStaked();
    if (total == 0) return "";

    // Use seed hash to pick deterministically
    std::string h = SHA256::hash(seed);
    uint64_t pick = 0;
    for (int i = 0; i < 8; i++)
        pick = (pick << 8) | (uint8_t)h[i];
    pick %= total;

    uint64_t cumulative = 0;
    for (const auto& [addr, v] : validators) {
        if (!v.active) continue;
        cumulative += v.stake;
        if (pick < cumulative) return addr;
    }
    return validators.begin()->first;
}

bool ProofOfStake::validateBlock(const std::string& validatorAddress,
                                  const std::string& blockHash) const {
    auto it = validators.find(validatorAddress);
    if (it == validators.end() || !it->second.active) return false;
    (void)blockHash;
    return true;
}

void ProofOfStake::distributeRewards(uint64_t totalReward) {
    uint64_t total = totalStaked();
    if (total == 0) return;

    for (auto& [addr, v] : validators) {
        if (!v.active) continue;
        // Reward proportional to stake
        uint64_t reward = (totalReward * v.stake) / total;
        v.rewards += reward;
    }
}

uint64_t ProofOfStake::totalStaked() const {
    uint64_t total = 0;
    for (const auto& [addr, v] : validators)
        if (v.active) total += v.stake;
    return total;
}

std::vector<Validator> ProofOfStake::getValidators() const {
    std::vector<Validator> result;
    for (const auto& [addr, v] : validators)
        result.push_back(v);
    return result;
}

void ProofOfStake::print() const {
    std::cout << "\n=== PoS Validators ===\n";
    for (const auto& [addr, v] : validators)
        std::cout << "  " << addr.substr(0,20)
                  << " stake=" << v.stake / 100000000.0 << " RXC"
                  << (v.active ? " [ACTIVE]" : " [INACTIVE]") << "\n";
    std::cout << "  Total staked: " << totalStaked() / 100000000.0 << " RXC\n";
}
