#include "stealth.h"
#include "../crypto/sha256.h"
#include "../crypto/ecdsa.h"
#include <iostream>

StealthAddress StealthAddressSystem::generateStealthAddress(
    const std::string& spendPrivKey,
    const std::string& viewPrivKey)
{
    StealthAddress sa;
    sa.spendPubKey = ECDSA::derivePublicKey(spendPrivKey);
    sa.viewPubKey  = ECDSA::derivePublicKey(viewPrivKey);
    return sa;
}

OneTimeAddress StealthAddressSystem::createOneTimeAddress(
    const StealthAddress& recipient)
{
    // Generate ephemeral key pair
    KeyPair ephemeral = ECDSA::generateKeyPair();

    // One-time address = H(ephemeralPriv * viewPub) * G + spendPub
    // Simplified: hash combination
    std::string shared  = SHA256::hash(ephemeral.privateKey + recipient.viewPubKey);
    std::string oneTime = SHA256::hash(shared + recipient.spendPubKey);

    OneTimeAddress ota;
    ota.address   = "RXC" + oneTime.substr(0, 33);
    ota.txPubKey  = ephemeral.publicKey;
    return ota;
}

bool StealthAddressSystem::scanTransaction(
    const std::string& viewPrivKey,
    const std::string& txPubKey,
    const std::string& outputAddress)
{
    // Recompute what our one-time address should be
    std::string shared    = SHA256::hash(viewPrivKey + txPubKey);
    std::string spendPub  = ECDSA::derivePublicKey(viewPrivKey);
    std::string expected  = "RXC" + SHA256::hash(shared + spendPub).substr(0, 33);
    return expected == outputAddress;
}

std::string StealthAddressSystem::deriveSpendKey(
    const std::string& spendPrivKey,
    const std::string& viewPrivKey,
    const std::string& txPubKey)
{
    std::string shared = SHA256::hash(viewPrivKey + txPubKey);
    // Spend key = spendPrivKey + H(shared)  (mod curve order in real impl)
    return SHA256::hash(spendPrivKey + shared);
}
