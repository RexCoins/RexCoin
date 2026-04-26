#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "../crypto/ecdsa.h"

// Transaction Input — spends a previous UTXO
struct TxInput {
    std::string prevTxHash;   // Hash of previous transaction
    uint32_t    outputIndex;  // Which output of that transaction
    std::string scriptSig;    // Unlocking script (signature + pubkey)
    Signature   signature;
};

// Transaction Output — creates a new UTXO
struct TxOutput {
    uint64_t    amount;       // Value in satoshis (1 RXC = 100,000,000)
    std::string address;      // Recipient address
    std::string scriptPubKey; // Locking script
};

struct Transaction {
    std::string            txHash;
    std::vector<TxInput>   inputs;
    std::vector<TxOutput>  outputs;
    int64_t                timestamp;
    bool                   isCoinbase;  // Mining reward transaction

    Transaction();

    // Coinbase transaction (mining reward)
    static Transaction createCoinbase(const std::string& minerAddress,
                                      uint64_t reward,
                                      uint32_t blockHeight);

    // Regular transaction
    static Transaction create(const std::vector<TxInput>& inputs,
                               const std::vector<TxOutput>& outputs);

    // Calculate transaction hash
    std::string calculateHash() const;

    // Get total input value
    uint64_t totalInput()  const;

    // Get total output value
    uint64_t totalOutput() const;

    // Get transaction fee
    uint64_t fee() const;

    // Validate transaction structure
    bool isValid() const;

    // Serialize to string
    std::string serialize() const;

    void print() const;
};
