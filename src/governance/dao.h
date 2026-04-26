#pragma once
#include "proposal.h"
#include <string>

// DAO — Decentralized Autonomous Organization
// Treasury management + governance bundled together
class DAO {
public:
    void addFunds(uint64_t amount);
    uint64_t getTreasury() const { return treasury; }
    bool spendTreasury(uint64_t amount, const std::string& purpose,
                        const std::string& recipient);
    GovernanceSystem& governance() { return gov; }
    void print() const;
private:
    uint64_t treasury = 0;
    GovernanceSystem gov;
};
