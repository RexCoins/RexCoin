#include "blockchain.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <sys/stat.h>

// ── Constructor ───────────────────────────────────────────────────────────────
Blockchain::Blockchain(const std::string& dir)
    : blockCache(512), currentDifficulty(4), dataDir(dir)
{
    lastAdjustmentTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    // Create data directory
    mkdir(dataDir.c_str(), 0755);

    // Open persistent database
    db.open(dataDir + "/chaindata");

    std::cout << "\033[36m[Chain] Initializing Rex Network...\033[0m\n";

    // Try to load existing chain from DB
    if (!loadFromDB()) {
        // Fresh start — create genesis block
        std::cout << "\033[36m[Chain] No existing chain — creating Genesis Block\033[0m\n";
        chain.push_back(createGenesisBlock());
        persistBlock(chain.back());
        db.setMeta("height",     "1");
        db.setMeta("bestHash",   chain.back().hash);
        db.setMeta("difficulty", std::to_string(currentDifficulty));
        std::cout << "\033[32m[Chain] Genesis Block created & saved to disk!\033[0m\n";
    } else {
        std::cout << "\033[32m[Chain] Loaded " << chain.size()
                  << " blocks from disk!\033[0m\n";
    }
}

Blockchain::~Blockchain() {
    db.setMeta("height",     std::to_string(chain.size()));
    db.setMeta("bestHash",   chain.empty() ? "" : chain.back().hash);
    db.setMeta("difficulty", std::to_string(currentDifficulty));
    db.close();
}

// ── Load from DB ─────────────────────────────────────────────────────────────
bool Blockchain::loadFromDB() {
    std::string heightStr;
    if (!db.getMeta("height", heightStr)) return false;
    uint32_t savedHeight = std::stoul(heightStr);
    if (savedHeight == 0) return false;

    // Load difficulty
    std::string diffStr;
    if (db.getMeta("difficulty", diffStr))
        currentDifficulty = std::stoul(diffStr);

    // We only load the last block header into memory for now
    // (full chain would be loaded on demand via DB)
    std::cout << "\033[36m[DB] Found existing chain: height="
              << savedHeight << "\033[0m\n";

    // For simplicity, recreate genesis so chain vector is not empty
    // In production, we'd deserialize all blocks
    chain.push_back(createGenesisBlock());
    return true;
}

// ── Genesis Block ─────────────────────────────────────────────────────────────
Block Blockchain::createGenesisBlock() {
    Transaction genesisTx = Transaction::createCoinbase(
        "RXCGenesisAddress_RexNetwork_2026", INITIAL_REWARD, 0);
    std::vector<Transaction> txs = {genesisTx};
    Block genesis(0, std::string(64,'0'), txs, currentDifficulty);
    genesis.mine();
    updateUTXOs(genesis);
    return genesis;
}

// ── Serialize Block ───────────────────────────────────────────────────────────
std::string Blockchain::serializeBlock(const Block& b) const {
    std::stringstream ss;
    ss << b.index << "|"
       << b.hash  << "|"
       << b.header.previousHash << "|"
       << b.header.merkleRoot   << "|"
       << b.header.timestamp    << "|"
       << b.header.difficulty   << "|"
       << b.header.nonce        << "|"
       << b.transactions.size();
    for (const auto& tx : b.transactions)
        ss << "|" << tx.txHash << "|" << tx.isCoinbase
           << "|" << tx.outputs.size();
    return ss.str();
}

// ── Persist Block to DB ───────────────────────────────────────────────────────
void Blockchain::persistBlock(const Block& b) {
    DBBatch batch;
    // Store block data
    batch.put("b:" + b.hash, serializeBlock(b));
    // Height index
    batch.put("h:" + std::to_string(b.index), b.hash);
    // Store each TX
    for (const auto& tx : b.transactions) {
        std::stringstream ts;
        ts << tx.txHash << "|" << tx.isCoinbase
           << "|" << tx.timestamp;
        for (const auto& out : tx.outputs)
            ts << "|OUT:" << out.address << ":" << out.amount;
        batch.put("t:" + tx.txHash, ts.str());

        // Store UTXOs
        for (uint32_t i = 0; i < tx.outputs.size(); i++) {
            std::string utxoData = tx.outputs[i].address + "|"
                + std::to_string(tx.outputs[i].amount) + "|0"; // 0=unspent
            batch.put("u:" + tx.txHash + ":" + std::to_string(i), utxoData);
        }
    }
    db.writeBatch(batch);
}

// ── Add Block ─────────────────────────────────────────────────────────────────
void Blockchain::addBlock(const std::vector<Transaction>& txs) {
    if (chain.size() % DIFFICULTY_WINDOW == 0 && chain.size() > 0)
        adjustDifficulty();

    Block newBlock(
        (uint32_t)chain.size(),
        chain.back().hash,
        txs,
        currentDifficulty
    );
    newBlock.mine();
    updateUTXOs(newBlock);

    // Save to DB first (durability)
    persistBlock(newBlock);
    db.setMeta("height",   std::to_string(chain.size()+1));
    db.setMeta("bestHash", newBlock.hash);

    chain.push_back(newBlock);

    // Cache the block
    blockCache.put("blk:" + newBlock.hash, serializeBlock(newBlock));

    std::cout << "\033[32m[Chain] Block #" << newBlock.index
              << " saved to disk ✓\033[0m\n";
}

// ── Difficulty Adjustment ─────────────────────────────────────────────────────
void Blockchain::adjustDifficulty() {
    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t elapsed  = now - lastAdjustmentTime;
    int64_t expected = BLOCK_TIME_SECS * DIFFICULTY_WINDOW;
    if (elapsed < expected/4) currentDifficulty++;
    else if (elapsed > expected*4 && currentDifficulty > 1) currentDifficulty--;
    lastAdjustmentTime = now;
    db.setMeta("difficulty", std::to_string(currentDifficulty));
    std::cout << "\033[33m[Chain] Difficulty → " << currentDifficulty << "\033[0m\n";
}

// ── Update UTXO Set ───────────────────────────────────────────────────────────
void Blockchain::updateUTXOs(const Block& block) {
    DBBatch batch;
    for (const auto& tx : block.transactions) {
        for (const auto& in : tx.inputs) {
            utxoSet.spendUTXO(in.prevTxHash, in.outputIndex);
            batch.del("u:" + in.prevTxHash + ":" +
                      std::to_string(in.outputIndex));
        }
        for (uint32_t i = 0; i < tx.outputs.size(); i++) {
            UTXO u;
            u.txHash = tx.txHash; u.outputIndex = i;
            u.address = tx.outputs[i].address;
            u.amount  = tx.outputs[i].amount;
            u.spent   = false;
            utxoSet.addUTXO(u);
        }
    }
    if (db.isOpen()) db.writeBatch(batch);
}

// ── Queries ───────────────────────────────────────────────────────────────────
bool Blockchain::isValid() const {
    for (size_t i=1;i<chain.size();i++) {
        if (!chain[i].isValid())                         return false;
        if (chain[i].header.previousHash!=chain[i-1].hash) return false;
    }
    return true;
}

uint64_t Blockchain::getBlockReward() const {
    uint32_t halvings = (uint32_t)(chain.size()/HALVING_INTERVAL);
    if (halvings>=64) return 0;
    return INITIAL_REWARD >> halvings;
}

uint32_t Blockchain::getDifficulty()  const { return currentDifficulty; }
size_t   Blockchain::getHeight()      const { return chain.size(); }
const Block& Blockchain::getLatestBlock() const { return chain.back(); }

const Block* Blockchain::getBlock(uint32_t i) const {
    if (i>=chain.size()) return nullptr;
    return &chain[i];
}

bool Blockchain::getBlockByHash(const std::string& hash, std::string& data) const {
    // Check cache first
    if (blockCache.contains("blk:"+hash)) {
        std::string cached;
        const_cast<LRUCache&>(blockCache).get("blk:"+hash, cached);
        data = cached; return true;
    }
    return db.getBlock(hash, data);
}

uint64_t Blockchain::getBalance(const std::string& addr) const {
    return utxoSet.getBalance(addr);
}

void Blockchain::printInfo() const {
    std::cout << "\n╔══════════════════════════════════════════════╗\n";
    std::cout << "║          REX NETWORK STATUS                  ║\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    std::cout << "║  Height      : " << chain.size()         << "\n";
    std::cout << "║  Best Hash   : " << chain.back().hash.substr(0,24) << "...\n";
    std::cout << "║  Difficulty  : " << currentDifficulty    << "\n";
    std::cout << "║  Reward      : " << getBlockReward()     << " RXC\n";
    std::cout << "║  Chain Valid : " << (isValid()?"✅ YES":"❌ NO") << "\n";
    std::cout << "║  DB Path     : " << dataDir << "/chaindata\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
    db.printStats();
}
