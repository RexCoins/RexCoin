#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct KeyPair {
    std::string privateKey;   // 32-byte hex (256-bit)
    std::string publicKey;    // 33-byte compressed hex (secp256k1)
};

struct Signature {
    std::string r;            // 32-byte hex
    std::string s;            // 32-byte hex
    std::vector<uint8_t> der; // DER-encoded (for scripts)
};

class ECDSA {
public:
    static KeyPair    generateKeyPair();
    static Signature  sign(const std::string& privateKeyHex,
                           const std::string& messageHashHex);
    static bool       verify(const std::string& publicKeyHex,
                              const std::string& messageHashHex,
                              const Signature& sig);
    static std::string derivePublicKey(const std::string& privateKeyHex);
    static std::string publicKeyToAddress(const std::string& publicKeyHex);
    static std::vector<uint8_t> hexToBytes(const std::string& hex);
    static std::string bytesToHex(const std::vector<uint8_t>& bytes);
private:
    static std::string generateSecurePrivateKey();
};
