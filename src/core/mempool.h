#pragma once
#include "transaction.h"
#include <unordered_map>
#include <vector>
#include <cstdint>

// Mempool — holds unconfirmed transactions waiting to be mined
// Miners pick highest-fee transactions first (like Bitcoin)

class Mempool {
public:
    static constexpr size_t   MAX_SIZE      = 5000;       // max TXs
    static constexpr uint64_t MIN_FEE_RATE  = 1;          // sat/byte
    static constexpr size_t   MAX_BLOCK_TXS = 2000;       // TXs per block

    // Add transaction to mempool
    bool add(const Transaction& tx);

    // Remove confirmed transaction
    void remove(const std::string& txHash);

    // Remove multiple confirmed TXs
    void removeConfirmed(const std::vector<std::string>& txHashes);

    // Get top N transactions by fee rate (for mining)
    std::vector<Transaction> getTopTxs(size_t count = MAX_BLOCK_TXS) const;

    // Check if TX exists in mempool
    bool contains(const std::string& txHash) const;

    // Get transaction by hash
    const Transaction* get(const std::string& txHash) const;

    // Current mempool size
    size_t size() const { return txs.size(); }

    // Is mempool empty?
    bool empty() const { return txs.empty(); }

    // Total fees in mempool
    uint64_t totalFees() const;

    // Clear mempool
    void clear() { txs.clear(); }

    void print() const;

private:
    std::unordered_map<std::string, Transaction> txs;

    // Fee rate = fee / tx_size (sat/byte)
    uint64_t feeRate(const Transaction& tx) const;
};
