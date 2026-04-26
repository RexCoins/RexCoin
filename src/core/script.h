#pragma once
#include <string>
#include <vector>

// Bitcoin-compatible Script system
// P2PKH = Pay to Public Key Hash (most common)
// P2SH  = Pay to Script Hash

enum class ScriptType { P2PKH, P2SH, P2PK, UNKNOWN };

class Script {
public:
    // Build P2PKH locking script: OP_DUP OP_HASH160 <hash160> OP_EQUALVERIFY OP_CHECKSIG
    static std::string buildP2PKH(const std::string& pubKeyHash);

    // Build P2SH locking script
    static std::string buildP2SH(const std::string& scriptHash);

    // Build P2PKH unlocking script: <sig> <pubkey>
    static std::string buildUnlockP2PKH(const std::string& sigHex,
                                         const std::string& pubKeyHex);

    // Verify a P2PKH script pair
    static bool verifyP2PKH(const std::string& lockScript,
                             const std::string& unlockScript,
                             const std::string& txHash);

    // Detect script type
    static ScriptType detectType(const std::string& script);

    // Extract address from locking script
    static std::string extractAddress(const std::string& lockScript);
};
