#include "script.h"
#include "../crypto/ecdsa.h"
#include "../crypto/hash160.h"

std::string Script::buildP2PKH(const std::string& pubKeyHash) {
    return "OP_DUP OP_HASH160 " + pubKeyHash + " OP_EQUALVERIFY OP_CHECKSIG";
}

std::string Script::buildP2SH(const std::string& scriptHash) {
    return "OP_HASH160 " + scriptHash + " OP_EQUAL";
}

std::string Script::buildUnlockP2PKH(const std::string& sigHex,
                                       const std::string& pubKeyHex) {
    return sigHex + " " + pubKeyHex;
}

bool Script::verifyP2PKH(const std::string& lockScript,
                          const std::string& unlockScript,
                          const std::string& txHash) {
    // Extract public key hash from lock script
    // Format: "OP_DUP OP_HASH160 <hash> OP_EQUALVERIFY OP_CHECKSIG"
    size_t start = lockScript.find("OP_HASH160 ") + 11;
    size_t end   = lockScript.find(" OP_EQUALVERIFY");
    if (start == std::string::npos || end == std::string::npos) return false;
    std::string expectedHash = lockScript.substr(start, end - start);

    // Extract sig and pubkey from unlock script: "<sig> <pubkey>"
    size_t sp = unlockScript.find(' ');
    if (sp == std::string::npos) return false;
    std::string sigHex    = unlockScript.substr(0, sp);
    std::string pubKeyHex = unlockScript.substr(sp + 1);

    // Verify HASH160(pubkey) == expectedHash
    auto pubBytes = ECDSA::hexToBytes(pubKeyHex);
    std::string computedHash = Hash160::hash(
        std::string(pubBytes.begin(), pubBytes.end()));
    if (computedHash != expectedHash) return false;

    // Verify signature
    Signature sig;
    sig.r = sigHex.substr(0, 64);
    sig.s = sigHex.substr(64, 64);
    return ECDSA::verify(pubKeyHex, txHash, sig);
}

ScriptType Script::detectType(const std::string& script) {
    if (script.find("OP_DUP OP_HASH160") != std::string::npos)
        return ScriptType::P2PKH;
    if (script.find("OP_HASH160") != std::string::npos &&
        script.find("OP_EQUAL") != std::string::npos)
        return ScriptType::P2SH;
    if (script.find("OP_CHECKSIG") != std::string::npos)
        return ScriptType::P2PK;
    return ScriptType::UNKNOWN;
}

std::string Script::extractAddress(const std::string& lockScript) {
    if (detectType(lockScript) == ScriptType::P2PKH) {
        size_t start = lockScript.find("OP_HASH160 ") + 11;
        size_t end   = lockScript.find(" OP_EQUALVERIFY");
        if (start != std::string::npos && end != std::string::npos)
            return lockScript.substr(start, end - start);
    }
    return "";
}
