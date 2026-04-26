#include "zkproof.h"
#include "../crypto/sha256.h"
#include <iostream>
#include <random>
#include <sstream>

// NOTE: Full zk-SNARK requires libsnark or bellman library.
// This scaffold implements the interface with cryptographic hashes
// as placeholders — real ZK math runs on elliptic curve pairings.

static std::string randomBytes(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    std::string result;
    for (int i = 0; i < n; i++) {
        std::stringstream ss;
        ss << std::hex << dist(gen);
        result += ss.str();
    }
    return result;
}

ZKProof ZKProofSystem::generateProof(const std::string& secret,
                                      const std::string& publicInput) {
    ZKProof proof;
    // Simulate proof components via hash chaining
    proof.pi_a       = SHA256::hash(secret + publicInput + "A");
    proof.pi_b       = SHA256::hash(secret + publicInput + "B");
    proof.pi_c       = SHA256::hash(proof.pi_a + proof.pi_b);
    proof.publicInput= publicInput;
    return proof;
}

bool ZKProofSystem::verify(const ZKProof& proof) {
    // Check structural consistency (placeholder)
    std::string expected = SHA256::hash(proof.pi_a + proof.pi_b);
    return expected == proof.pi_c;
}

std::string ZKProofSystem::generateNullifier(const std::string& secret,
                                              const std::string& txHash) {
    return SHA256::hash(SHA256::hash(secret + txHash));
}

std::string ZKProofSystem::generateCommitment(uint64_t amount,
                                               const std::string& randomness) {
    return SHA256::hash(std::to_string(amount) + randomness);
}

ShieldedTx ZKProofSystem::createShieldedTx(
    const std::string& senderPrivKey,
    const std::string& recipientAddr,
    uint64_t amount,
    uint64_t fee)
{
    ShieldedTx tx;
    std::string rand = randomBytes(16);
    tx.commitment = generateCommitment(amount, rand);
    tx.nullifier  = generateNullifier(senderPrivKey, tx.commitment);
    tx.proof      = generateProof(senderPrivKey,
                                   tx.commitment + recipientAddr);
    tx.fee        = fee;

    std::cout << "\033[36m[ZK] Shielded TX created\n"
              << "     commitment=" << tx.commitment.substr(0,32) << "...\n"
              << "     nullifier="  << tx.nullifier.substr(0,32)  << "...\033[0m\n";
    return tx;
}

bool ZKProofSystem::verifyShieldedTx(const ShieldedTx& tx) {
    bool ok = verify(tx.proof);
    std::cout << "\033[" << (ok ? "32" : "31") << "m[ZK] Proof "
              << (ok ? "VALID ✓" : "INVALID ✗") << "\033[0m\n";
    return ok;
}
