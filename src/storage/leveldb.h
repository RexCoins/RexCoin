#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
//  BlockchainDB — Persistent storage using LevelDB (or in-memory fallback)
//
//  Key schema (same concept as Bitcoin Core chainstate):
//  "b:{hash}"        → serialized Block
//  "t:{hash}"        → serialized Transaction
//  "u:{txhash}:{i}"  → UTXO entry
//  "h:{height}"      → block hash at height
//  "meta:{key}"      → metadata (height, bestHash, difficulty)
// ─────────────────────────────────────────────────────────────────────────────

struct DBBatch {
    std::vector<std::pair<std::string,std::string>> puts;
    std::vector<std::string>                        dels;
    void put(const std::string& k, const std::string& v){ puts.push_back({k,v}); }
    void del(const std::string& k){ dels.push_back(k); }
};

class BlockchainDB {
public:
    BlockchainDB();
    ~BlockchainDB();

    bool open(const std::string& path);
    void close();
    bool isOpen() const { return opened; }

    // Basic KV
    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;
    bool del(const std::string& key);
    bool exists(const std::string& key) const;

    // Atomic batch write
    bool writeBatch(const DBBatch& batch);

    // Prefix scan
    std::vector<std::pair<std::string,std::string>>
        scan(const std::string& prefix) const;
    std::vector<std::string> keys(const std::string& prefix = "") const;

    // Blockchain helpers
    bool putBlock(const std::string& hash, const std::string& data);
    bool getBlock(const std::string& hash, std::string& data) const;
    bool putTx(const std::string& hash, const std::string& data);
    bool getTx(const std::string& hash, std::string& data) const;
    bool putUTXO(const std::string& txHash, uint32_t idx, const std::string& data);
    bool getUTXO(const std::string& txHash, uint32_t idx, std::string& data) const;
    bool delUTXO(const std::string& txHash, uint32_t idx);
    bool putHeightIndex(uint32_t height, const std::string& hash);
    bool getHashAtHeight(uint32_t height, std::string& hash) const;
    bool setMeta(const std::string& key, const std::string& value);
    bool getMeta(const std::string& key, std::string& value) const;

    size_t size() const;
    void   printStats() const;

private:
    bool    opened = false;
    std::string dbPath;

    // In-memory backend (used when LevelDB not available)
    mutable std::unordered_map<std::string,std::string> memdb;

    static std::string utxoKey(const std::string& tx, uint32_t idx);
    static std::string heightKey(uint32_t h);
};
