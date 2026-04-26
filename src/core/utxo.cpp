#include "utxo.h"
#include <iostream>

void UTXOSet::addUTXO(const UTXO& utxo) {
    utxos[utxo.key()] = utxo;
}

bool UTXOSet::spendUTXO(const std::string& txHash, uint32_t outputIndex) {
    UTXO temp; temp.txHash = txHash; temp.outputIndex = outputIndex;
    auto it = utxos.find(temp.key());
    if (it == utxos.end() || it->second.spent) return false;
    it->second.spent = true;
    return true;
}

std::vector<UTXO> UTXOSet::getUTXOs(const std::string& address) const {
    std::vector<UTXO> result;
    for (const auto& [key, utxo] : utxos)
        if (utxo.address == address && !utxo.spent)
            result.push_back(utxo);
    return result;
}

uint64_t UTXOSet::getBalance(const std::string& address) const {
    uint64_t total = 0;
    for (const auto& [key, utxo] : utxos)
        if (utxo.address == address && !utxo.spent)
            total += utxo.amount;
    return total;
}

bool UTXOSet::isUnspent(const std::string& txHash, uint32_t outputIndex) const {
    UTXO temp; temp.txHash = txHash; temp.outputIndex = outputIndex;
    auto it = utxos.find(temp.key());
    return it != utxos.end() && !it->second.spent;
}

const UTXO* UTXOSet::getUTXO(const std::string& txHash, uint32_t outputIndex) const {
    UTXO temp; temp.txHash = txHash; temp.outputIndex = outputIndex;
    auto it = utxos.find(temp.key());
    if (it == utxos.end()) return nullptr;
    return &it->second;
}

size_t UTXOSet::size() const { return utxos.size(); }

void UTXOSet::print() const {
    std::cout << "\n=== UTXO Set (" << utxos.size() << " entries) ===\n";
    for (const auto& [key, utxo] : utxos) {
        if (!utxo.spent)
            std::cout << "  [UNSPENT] " << utxo.address.substr(0,20)
                      << " : " << utxo.amount / 100000000.0 << " RXC\n";
    }
}
