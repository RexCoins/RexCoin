#include "mempool.h"
#include <algorithm>
#include <iostream>

bool Mempool::add(const Transaction& tx) {
    if (txs.size() >= MAX_SIZE) {
        std::cout << "\033[31m[Mempool] Full! Rejecting TX\033[0m\n";
        return false;
    }
    if (!tx.isValid()) {
        std::cout << "\033[31m[Mempool] Invalid TX rejected\033[0m\n";
        return false;
    }
    if (txs.count(tx.txHash)) return false; // duplicate

    txs[tx.txHash] = tx;
    std::cout << "\033[36m[Mempool] TX added: " << tx.txHash.substr(0,16)
              << "... (" << txs.size() << " pending)\033[0m\n";
    return true;
}

void Mempool::remove(const std::string& txHash) {
    txs.erase(txHash);
}

void Mempool::removeConfirmed(const std::vector<std::string>& hashes) {
    for (const auto& h : hashes) txs.erase(h);
    std::cout << "\033[32m[Mempool] Removed " << hashes.size()
              << " confirmed TXs. Remaining: " << txs.size() << "\033[0m\n";
}

std::vector<Transaction> Mempool::getTopTxs(size_t count) const {
    std::vector<const Transaction*> ptrs;
    for (const auto& [h, tx] : txs) ptrs.push_back(&tx);

    // Sort by fee rate descending (miners maximize fees)
    std::sort(ptrs.begin(), ptrs.end(), [this](const Transaction* a, const Transaction* b) {
        return feeRate(*a) > feeRate(*b);
    });

    std::vector<Transaction> result;
    for (size_t i = 0; i < std::min(count, ptrs.size()); i++)
        result.push_back(*ptrs[i]);
    return result;
}

bool Mempool::contains(const std::string& txHash) const {
    return txs.count(txHash) > 0;
}

const Transaction* Mempool::get(const std::string& txHash) const {
    auto it = txs.find(txHash);
    return (it != txs.end()) ? &it->second : nullptr;
}

uint64_t Mempool::totalFees() const {
    uint64_t total = 0;
    for (const auto& [h, tx] : txs) total += tx.fee();
    return total;
}

uint64_t Mempool::feeRate(const Transaction& tx) const {
    size_t size = tx.serialize().size();
    if (size == 0) return 0;
    return tx.fee() / size;
}

void Mempool::print() const {
    std::cout << "\n╔══════════════════════════════╗\n";
    std::cout << "║  MEMPOOL                     ║\n";
    std::cout << "╠══════════════════════════════╣\n";
    std::cout << "║  Pending TXs : " << txs.size() << "\n";
    std::cout << "║  Total Fees  : " << totalFees() / 1e8 << " RXC\n";
    std::cout << "╚══════════════════════════════╝\n";
}
