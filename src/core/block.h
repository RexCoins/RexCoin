#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "transaction.h"

struct BlockHeader {
    uint32_t    version       = 1;
    std::string previousHash;
    std::string merkleRoot;
    int64_t     timestamp;
    uint32_t    difficulty;
    uint64_t    nonce         = 0;
};

struct Block {
    uint32_t                 index;
    BlockHeader              header;
    std::vector<Transaction> transactions;
    std::string              hash;

    Block(uint32_t index,
          const std::string& previousHash,
          const std::vector<Transaction>& txs,
          uint32_t difficulty);

    std::string calculateHash() const;
    void        mine();
    bool        isValid()  const;
    size_t      txCount()  const;
    void        print()    const;
};
