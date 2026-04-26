#include "dao.h"
#include <iostream>

void DAO::addFunds(uint64_t amount) {
    treasury += amount;
    std::cout << "[DAO] Treasury: " << treasury/1e8 << " RXC\n";
}

bool DAO::spendTreasury(uint64_t amount, const std::string& purpose,
                         const std::string& recipient) {
    if (amount > treasury) {
        std::cout << "[DAO] Insufficient treasury\n"; return false;
    }
    treasury -= amount;
    std::cout << "[DAO] Spent " << amount/1e8 << " RXC for: "
              << purpose << " → " << recipient.substr(0,16) << "\n";
    return true;
}

void DAO::print() const {
    std::cout << "\n=== DAO ===\n";
    std::cout << "  Treasury: " << treasury/1e8 << " RXC\n";
    gov.print();
}
