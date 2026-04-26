#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

// UTXO — Unspent Transaction Output
struct UTXO {
    std::string txHash;
    uint32_t    outputIndex;
    std::string address;
    uint64_t    amount;       // In satoshis
    bool        spent;

    std::string key() const {
        return txHash + ":" + std::to_string(outputIndex);
    }
};

// UTXO Set — tracks all unspent outputs (like Bitcoin's chainstate)
class UTXOSet {
public:
    // Add a new UTXO
    void addUTXO(const UTXO& utxo);

    // Mark a UTXO as spent
    bool spendUTXO(const std::string& txHash, uint32_t outputIndex);

    // Get all UTXOs for an address
    std::vector<UTXO> getUTXOs(const std::string& address) const;

    // Get balance of an address
    uint64_t getBalance(const std::string& address) const;

    // Check if UTXO exists and is unspent
    bool isUnspent(const std::string& txHash, uint32_t outputIndex) const;

    // Get UTXO by key
    const UTXO* getUTXO(const std::string& txHash, uint32_t outputIndex) const;

    // Total UTXOs in set
    size_t size() const;

    void print() const;

private:
    std::unordered_map<std::string, UTXO> utxos; // key → UTXO
};
