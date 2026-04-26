#pragma once
#include <string>
#include <vector>

// BLS (Boneh-Lynn-Shacham) Signatures
// Advanced feature: allows signature aggregation
// Multiple signatures → 1 signature (saves block space)
struct BLSKeyPair {
    std::string privateKey;
    std::string publicKey;
};

class BLS {
public:
    static BLSKeyPair generateKeyPair();
    static std::string sign(const std::string& privateKey,
                            const std::string& message);
    static bool verify(const std::string& publicKey,
                       const std::string& message,
                       const std::string& signature);

    // KEY FEATURE: Aggregate multiple signatures into one
    static std::string aggregate(const std::vector<std::string>& signatures);
    static bool verifyAggregate(const std::vector<std::string>& publicKeys,
                                const std::vector<std::string>& messages,
                                const std::string& aggSignature);
};
