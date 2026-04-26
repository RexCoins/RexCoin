#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Zero-Knowledge Proof — proves you know a value without revealing it
// Used for shielded (private) transactions in RexCoin
// Based on zk-SNARKs concept (like Zcash's Groth16)

struct ZKProof {
    std::string pi_a;   // Proof component A
    std::string pi_b;   // Proof component B
    std::string pi_c;   // Proof component C
    std::string publicInput;
};

struct ShieldedTx {
    std::string  nullifier;     // Prevents double-spend
    std::string  commitment;    // Hides the actual amount
    ZKProof      proof;         // Proves tx is valid without revealing details
    uint64_t     fee;           // Public fee
};

class ZKProofSystem {
public:
    // Generate a proof that you know 'secret' value producing 'commitment'
    static ZKProof generateProof(const std::string& secret,
                                  const std::string& publicInput);

    // Verify a proof without learning the secret
    static bool verify(const ZKProof& proof);

    // Create a shielded transaction (private amount + address)
    static ShieldedTx createShieldedTx(
        const std::string& senderPrivKey,
        const std::string& recipientAddr,
        uint64_t amount,
        uint64_t fee);

    // Verify a shielded transaction
    static bool verifyShieldedTx(const ShieldedTx& tx);

    // Generate nullifier (prevents double-spend)
    static std::string generateNullifier(const std::string& secret,
                                          const std::string& txHash);

    // Generate commitment (hides amount)
    static std::string generateCommitment(uint64_t amount,
                                           const std::string& randomness);
};
