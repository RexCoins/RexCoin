#pragma once
#include "block.h"
#include "utxo.h"
#include "transaction.h"
#include "../storage/leveldb.h"
#include "../storage/cache.h"
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

class Blockchain {
public:
    // ── RexCoin Constants ─────────────────────────────────────────────────
    static constexpr uint32_t HALVING_INTERVAL  = 210000;
    static constexpr uint64_t INITIAL_REWARD    = 50;
    static constexpr uint32_t BLOCK_TIME_SECS   = 600;
    static constexpr uint32_t DIFFICULTY_WINDOW = 2016;
    static constexpr uint64_t MAX_SUPPLY        = 21000000ULL;
    static constexpr uint64_t SATOSHIS          = 100000000ULL;
    static constexpr uint32_t MAX_BLOCK_SIZE    = 1000000;

    // Open or create blockchain at path
    explicit Blockchain(const std::string& dataDir = ".rexcoin");
    ~Blockchain();

    // Add a new block
    void addBlock(const std::vector<Transaction>& txs);

    // Validate entire chain
    bool isValid() const;

    // Block reward at current height
    uint64_t getBlockReward() const;

    // Current mining difficulty
    uint32_t getDifficulty() const;

    // Chain height
    size_t getHeight() const;

    // Get latest block
    const Block& getLatestBlock() const;

    // Get block by index
    const Block* getBlock(uint32_t index) const;

    // Get block by hash (from DB)
    bool getBlockByHash(const std::string& hash, std::string& data) const;

    // UTXO balance
    uint64_t getBalance(const std::string& address) const;

    // DB reference
    BlockchainDB& getDB() { return db; }

    void printInfo() const;

private:
    std::vector<Block> chain;      // In-memory chain (recent blocks)
    UTXOSet            utxoSet;
    BlockchainDB       db;
    LRUCache           blockCache; // LRU cache for block lookups
    uint32_t           currentDifficulty;
    int64_t            lastAdjustmentTime;
    std::string        dataDir;

    Block createGenesisBlock();
    void  adjustDifficulty();
    void  updateUTXOs(const Block& block);

    // Serialization helpers
    std::string serializeBlock(const Block& b) const;
    void        persistBlock(const Block& b);
    bool        loadFromDB();
};
