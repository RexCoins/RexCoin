#include "merkle.h"
#include "../crypto/sha256.h"
#include <algorithm>

std::string MerkleTree::hashPair(const std::string& a, const std::string& b) {
    return SHA256::hash(SHA256::hash(a + b));
}

std::string MerkleTree::buildRoot(const std::vector<std::string>& txHashes) {
    if (txHashes.empty()) return std::string(64, '0');
    if (txHashes.size() == 1) return txHashes[0];

    std::vector<std::string> layer = txHashes;

    while (layer.size() > 1) {
        // If odd number, duplicate last element (like Bitcoin)
        if (layer.size() % 2 != 0)
            layer.push_back(layer.back());

        std::vector<std::string> nextLayer;
        for (size_t i = 0; i < layer.size(); i += 2)
            nextLayer.push_back(hashPair(layer[i], layer[i+1]));
        layer = nextLayer;
    }
    return layer[0];
}

std::vector<std::string> MerkleTree::generateProof(
    const std::vector<std::string>& txHashes,
    const std::string& txHash)
{
    std::vector<std::string> proof;
    std::vector<std::string> layer = txHashes;

    // Find index of target tx
    auto it = std::find(layer.begin(), layer.end(), txHash);
    if (it == layer.end()) return proof;
    size_t idx = std::distance(layer.begin(), it);

    while (layer.size() > 1) {
        if (layer.size() % 2 != 0) layer.push_back(layer.back());

        // Sibling index
        size_t sibling = (idx % 2 == 0) ? idx + 1 : idx - 1;
        proof.push_back(layer[sibling]);

        std::vector<std::string> nextLayer;
        for (size_t i = 0; i < layer.size(); i += 2)
            nextLayer.push_back(hashPair(layer[i], layer[i+1]));

        layer = nextLayer;
        idx /= 2;
    }
    return proof;
}

bool MerkleTree::verifyProof(
    const std::string& root,
    const std::string& txHash,
    const std::vector<std::string>& proof)
{
    std::string current = txHash;
    for (const auto& sibling : proof)
        current = hashPair(current, sibling);
    return current == root;
}
