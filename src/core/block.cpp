#include "block.h"
#include "merkle.h"
#include "../crypto/sha256.h"
#include <iostream>
#include <sstream>
#include <chrono>

Block::Block(uint32_t idx,
             const std::string& prevHash,
             const std::vector<Transaction>& txs,
             uint32_t difficulty)
    : index(idx), transactions(txs)
{
    header.previousHash = prevHash;
    header.difficulty   = difficulty;
    header.timestamp    = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    // Build Merkle root from transaction hashes
    std::vector<std::string> txHashes;
    for (const auto& tx : transactions) txHashes.push_back(tx.txHash);
    header.merkleRoot = MerkleTree::buildRoot(txHashes);

    hash = calculateHash();
}

std::string Block::calculateHash() const {
    std::stringstream ss;
    ss << header.version
       << header.previousHash
       << header.merkleRoot
       << header.timestamp
       << header.difficulty
       << header.nonce;
    return SHA256::hash(SHA256::hash(ss.str())); // Double SHA256
}

void Block::mine() {
    std::string target(header.difficulty, '0');
    uint64_t attempts = 0;

    std::cout << "\033[36m[i] Mining Block #" << index
              << " (difficulty=" << header.difficulty << ")...\033[0m\n";

    while (hash.substr(0, header.difficulty) != target) {
        header.nonce++;
        hash = calculateHash();
        attempts++;
    }

    std::cout << "\033[32m[✓] Block #" << index << " mined!"
              << "  nonce=" << header.nonce
              << "  attempts=" << attempts
              << "\n    hash=" << hash << "\033[0m\n";
}

bool Block::isValid() const {
    std::string target(header.difficulty, '0');
    return hash == calculateHash() &&
           hash.substr(0, header.difficulty) == target;
}

size_t Block::txCount() const { return transactions.size(); }

void Block::print() const {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║  Block #" << index << "\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    std::cout << "║  Hash       : " << hash.substr(0,40) << "...\n";
    std::cout << "║  PrevHash   : " << header.previousHash.substr(0,40) << "...\n";
    std::cout << "║  MerkleRoot : " << header.merkleRoot.substr(0,40)   << "...\n";
    std::cout << "║  Nonce      : " << header.nonce      << "\n";
    std::cout << "║  Difficulty : " << header.difficulty << "\n";
    std::cout << "║  TXs        : " << transactions.size() << "\n";
    std::cout << "╚══════════════════════════════════════════╝\n";
    for (const auto& tx : transactions) tx.print();
}
