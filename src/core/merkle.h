#pragma once
#include <string>
#include <vector>

// Merkle Tree — proves transactions are in a block (like Bitcoin)
class MerkleTree {
public:
    // Build tree from transaction hashes, return root
    static std::string buildRoot(const std::vector<std::string>& txHashes);

    // Generate proof that a TX is in the tree
    static std::vector<std::string> generateProof(
        const std::vector<std::string>& txHashes,
        const std::string& txHash);

    // Verify a merkle proof
    static bool verifyProof(
        const std::string& root,
        const std::string& txHash,
        const std::vector<std::string>& proof);

private:
    static std::string hashPair(const std::string& a, const std::string& b);
};
