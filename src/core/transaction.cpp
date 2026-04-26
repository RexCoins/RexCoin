#include "transaction.h"
#include "../crypto/sha256.h"
#include <sstream>
#include <iostream>
#include <chrono>

Transaction::Transaction() : timestamp(0), isCoinbase(false) {
    timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

Transaction Transaction::createCoinbase(const std::string& minerAddress,
                                         uint64_t reward,
                                         uint32_t blockHeight) {
    Transaction tx;
    tx.isCoinbase = true;

    // Coinbase has no real input
    TxInput cbInput;
    cbInput.prevTxHash   = std::string(64, '0');
    cbInput.outputIndex  = 0xFFFFFFFF;
    cbInput.scriptSig    = "RexCoin Block #" + std::to_string(blockHeight);
    tx.inputs.push_back(cbInput);

    // Output: reward goes to miner
    TxOutput cbOutput;
    cbOutput.amount  = reward * 100000000ULL; // Convert to satoshis
    cbOutput.address = minerAddress;
    cbOutput.scriptPubKey = "OP_DUP OP_HASH160 " + minerAddress + " OP_EQUALVERIFY OP_CHECKSIG";
    tx.outputs.push_back(cbOutput);

    tx.txHash = tx.calculateHash();
    return tx;
}

Transaction Transaction::create(const std::vector<TxInput>& ins,
                                 const std::vector<TxOutput>& outs) {
    Transaction tx;
    tx.inputs  = ins;
    tx.outputs = outs;
    tx.txHash  = tx.calculateHash();
    return tx;
}

std::string Transaction::calculateHash() const {
    std::stringstream ss;
    ss << timestamp << isCoinbase;
    for (const auto& in  : inputs)
        ss << in.prevTxHash << in.outputIndex;
    for (const auto& out : outputs)
        ss << out.amount << out.address;
    return SHA256::hash(SHA256::hash(ss.str())); // Double SHA256 like Bitcoin
}

uint64_t Transaction::totalInput()  const {
    // UTXO lookup needed for real value — return 0 as placeholder
    return 0;
}

uint64_t Transaction::totalOutput() const {
    uint64_t total = 0;
    for (const auto& out : outputs) total += out.amount;
    return total;
}

uint64_t Transaction::fee() const {
    if (isCoinbase) return 0;
    uint64_t in = totalInput();
    uint64_t out = totalOutput();
    return (in > out) ? (in - out) : 0;
}

bool Transaction::isValid() const {
    if (outputs.empty()) return false;
    if (!isCoinbase && inputs.empty()) return false;
    return true;
}

std::string Transaction::serialize() const {
    std::stringstream ss;
    ss << txHash << "|" << timestamp << "|" << isCoinbase
       << "|inputs:" << inputs.size()
       << "|outputs:" << outputs.size();
    return ss.str();
}

void Transaction::print() const {
    std::cout << "  TX: " << txHash.substr(0,32) << "...\n";
    std::cout << "  Outputs: " << outputs.size() << "\n";
    for (const auto& out : outputs)
        std::cout << "    → " << out.address.substr(0,20)
                  << " : " << out.amount / 100000000.0 << " RXC\n";
}
