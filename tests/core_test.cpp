// Core Tests
#include <iostream>
#include <cassert>
#include "../src/core/block.h"
#include "../src/core/blockchain.h"
#include "../src/core/transaction.h"
#include "../src/core/merkle.h"
#include "../src/crypto/sha256.h"

void test_sha256() {
    std::string result = SHA256::hash("hello");
    assert(result == "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824");
    std::cout << "[PASS] SHA256\n";
}

void test_merkle() {
    std::vector<std::string> txs = {"tx1","tx2","tx3","tx4"};
    std::string root = MerkleTree::buildRoot(txs);
    assert(!root.empty() && root.size() == 64);

    auto proof = MerkleTree::generateProof(txs, "tx1");
    assert(MerkleTree::verifyProof(root, "tx1", proof));
    std::cout << "[PASS] Merkle Tree\n";
}

void test_transaction() {
    Transaction tx = Transaction::createCoinbase("RXCTestAddress", 50, 0);
    assert(tx.isCoinbase);
    assert(!tx.txHash.empty());
    assert(tx.isValid());
    std::cout << "[PASS] Transaction\n";
}

void test_blockchain() {
    Blockchain chain;
    assert(chain.getHeight() == 1); // Genesis block

    Transaction tx = Transaction::createCoinbase("RXCMiner", 50, 1);
    chain.addBlock({tx});
    assert(chain.getHeight() == 2);
    assert(chain.isValid());
    std::cout << "[PASS] Blockchain\n";
}

int main() {
    std::cout << "\n=== RexCoin Core Tests ===\n\n";
    test_sha256();
    test_merkle();
    test_transaction();
    test_blockchain();
    std::cout << "\n[ALL TESTS PASSED] ✓\n";
    return 0;
}
