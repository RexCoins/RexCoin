#include "leveldb.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef HAS_LEVELDB
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
static leveldb::DB* ldb = nullptr;
#endif

BlockchainDB::BlockchainDB() {}
BlockchainDB::~BlockchainDB() { close(); }

// ── Static key builders ──────────────────────────────────────────────────────
std::string BlockchainDB::utxoKey(const std::string& tx, uint32_t idx) {
    return "u:" + tx + ":" + std::to_string(idx);
}
std::string BlockchainDB::heightKey(uint32_t h) {
    std::stringstream ss;
    ss << "h:" << std::setw(10) << std::setfill('0') << h;
    return ss.str();
}

// ── Open ─────────────────────────────────────────────────────────────────────
bool BlockchainDB::open(const std::string& path) {
    dbPath = path;

#ifdef HAS_LEVELDB
    leveldb::Options opts;
    opts.create_if_missing = true;
    opts.block_cache       = leveldb::NewLRUCache(64 * 1024 * 1024); // 64MB
    opts.filter_policy     = leveldb::NewBloomFilterPolicy(10);
    opts.write_buffer_size = 16 * 1024 * 1024; // 16MB
    leveldb::Status s = leveldb::DB::Open(opts, path, &ldb);
    if (!s.ok()) {
        std::cerr << "\033[31m[DB] LevelDB open failed: " << s.ToString()
                  << " — using in-memory fallback\033[0m\n";
        opened = true;
        std::cout << "\033[33m[DB] In-memory mode (install LevelDB for persistence)\033[0m\n";
        return true;
    }
    std::cout << "\033[32m[DB] LevelDB opened: " << path << "\033[0m\n";
#else
    std::cout << "\033[33m[DB] In-memory mode at: " << path
              << "\n     (run: pkg install leveldb && rebuild for persistence)\033[0m\n";
#endif

    opened = true;
    return true;
}

void BlockchainDB::close() {
    if (!opened) return;
#ifdef HAS_LEVELDB
    if (ldb) { delete ldb; ldb = nullptr; }
#endif
    opened = false;
    std::cout << "\033[33m[DB] Closed. Entries: " << memdb.size() << "\033[0m\n";
}

// ── Basic KV ─────────────────────────────────────────────────────────────────
bool BlockchainDB::put(const std::string& k, const std::string& v) {
    if (!opened) return false;
#ifdef HAS_LEVELDB
    if (ldb) {
        leveldb::Status s = ldb->Put(leveldb::WriteOptions(), k, v);
        return s.ok();
    }
#endif
    memdb[k] = v;
    return true;
}

bool BlockchainDB::get(const std::string& k, std::string& v) const {
    if (!opened) return false;
#ifdef HAS_LEVELDB
    if (ldb) {
        leveldb::Status s = ldb->Get(leveldb::ReadOptions(), k, &v);
        return s.ok();
    }
#endif
    auto it = memdb.find(k);
    if (it == memdb.end()) return false;
    v = it->second;
    return true;
}

bool BlockchainDB::del(const std::string& k) {
    if (!opened) return false;
#ifdef HAS_LEVELDB
    if (ldb) {
        leveldb::Status s = ldb->Delete(leveldb::WriteOptions(), k);
        return s.ok();
    }
#endif
    return memdb.erase(k) > 0;
}

bool BlockchainDB::exists(const std::string& k) const {
    std::string v;
    return get(k, v);
}

// ── Atomic Batch ─────────────────────────────────────────────────────────────
bool BlockchainDB::writeBatch(const DBBatch& batch) {
    if (!opened) return false;
#ifdef HAS_LEVELDB
    if (ldb) {
        leveldb::WriteBatch lb;
        for (const auto& [k,v] : batch.puts) lb.Put(k, v);
        for (const auto& k    : batch.dels)  lb.Delete(k);
        leveldb::WriteOptions wo; wo.sync = true;
        return ldb->Write(wo, &lb).ok();
    }
#endif
    // In-memory atomic batch
    for (const auto& [k,v] : batch.puts) memdb[k] = v;
    for (const auto& k    : batch.dels)  memdb.erase(k);
    return true;
}

// ── Prefix scan ──────────────────────────────────────────────────────────────
std::vector<std::pair<std::string,std::string>>
BlockchainDB::scan(const std::string& prefix) const {
    std::vector<std::pair<std::string,std::string>> result;
#ifdef HAS_LEVELDB
    if (ldb) {
        auto* it = ldb->NewIterator(leveldb::ReadOptions());
        for (it->Seek(prefix); it->Valid(); it->Next()) {
            std::string k = it->key().ToString();
            if (k.substr(0, prefix.size()) != prefix) break;
            result.push_back({k, it->value().ToString()});
        }
        delete it;
        return result;
    }
#endif
    for (const auto& [k,v] : memdb)
        if (k.substr(0, prefix.size()) == prefix)
            result.push_back({k, v});
    return result;
}

std::vector<std::string> BlockchainDB::keys(const std::string& prefix) const {
    auto pairs = scan(prefix);
    std::vector<std::string> ks;
    for (const auto& [k,v] : pairs) ks.push_back(k);
    return ks;
}

// ── Blockchain helpers ───────────────────────────────────────────────────────
bool BlockchainDB::putBlock(const std::string& h, const std::string& d)
    { return put("b:"+h, d); }
bool BlockchainDB::getBlock(const std::string& h, std::string& d) const
    { return get("b:"+h, d); }

bool BlockchainDB::putTx(const std::string& h, const std::string& d)
    { return put("t:"+h, d); }
bool BlockchainDB::getTx(const std::string& h, std::string& d) const
    { return get("t:"+h, d); }

bool BlockchainDB::putUTXO(const std::string& tx, uint32_t idx,
                             const std::string& d)
    { return put(utxoKey(tx,idx), d); }
bool BlockchainDB::getUTXO(const std::string& tx, uint32_t idx,
                             std::string& d) const
    { return get(utxoKey(tx,idx), d); }
bool BlockchainDB::delUTXO(const std::string& tx, uint32_t idx)
    { return del(utxoKey(tx,idx)); }

bool BlockchainDB::putHeightIndex(uint32_t h, const std::string& hash)
    { return put(heightKey(h), hash); }
bool BlockchainDB::getHashAtHeight(uint32_t h, std::string& hash) const
    { return get(heightKey(h), hash); }

bool BlockchainDB::setMeta(const std::string& k, const std::string& v)
    { return put("meta:"+k, v); }
bool BlockchainDB::getMeta(const std::string& k, std::string& v) const
    { return get("meta:"+k, v); }

size_t BlockchainDB::size() const {
#ifdef HAS_LEVELDB
    if (ldb) {
        std::string prop;
        ldb->GetProperty("leveldb.approximate-memory-usage", &prop);
    }
#endif
    return memdb.size();
}

void BlockchainDB::printStats() const {
    std::cout << "\n╔══════════════════════════════════╗\n";
    std::cout << "║  DATABASE STATS                  ║\n";
    std::cout << "╠══════════════════════════════════╣\n";
    std::cout << "║  Path    : " << dbPath << "\n";
    std::cout << "║  Entries : " << size() << "\n";
    std::cout << "║  Blocks  : " << scan("b:").size() << "\n";
    std::cout << "║  TXs     : " << scan("t:").size() << "\n";
    std::cout << "║  UTXOs   : " << scan("u:").size() << "\n";
#ifdef HAS_LEVELDB
    std::cout << "║  Engine  : LevelDB (persistent)\n";
#else
    std::cout << "║  Engine  : In-memory (install leveldb)\n";
#endif
    std::cout << "╚══════════════════════════════════╝\n";
}
