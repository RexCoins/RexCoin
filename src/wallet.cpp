#include "wallet.h"
#include "hash160.h"
#include <iostream>
#include <sstream>

Wallet::Wallet() : balance(0) {
    keyPair = ECDSA::generateKeyPair();
    // Real Bitcoin-style address: HASH160(pubkey) → Base58Check
    address = ECDSA::publicKeyToAddress(keyPair.publicKey);
}

Wallet::Wallet(const std::string& privKey) : balance(0) {
    keyPair.privateKey = privKey;
    keyPair.publicKey  = ECDSA::derivePublicKey(privKey);
    address = ECDSA::publicKeyToAddress(keyPair.publicKey);
}

std::string Wallet::getAddress()    const { return address; }
std::string Wallet::getPublicKey()  const { return keyPair.publicKey; }
std::string Wallet::getPrivateKey() const { return keyPair.privateKey; }
uint64_t    Wallet::getBalance()    const { return balance; }

Signature Wallet::signTransaction(const std::string& txHash) const {
    return ECDSA::sign(keyPair.privateKey, txHash);
}

bool Wallet::verifySignature(const std::string& txHash,
                              const Signature& sig) const {
    return ECDSA::verify(keyPair.publicKey, txHash, sig);
}

void Wallet::setBalance(uint64_t bal) { balance = bal; }

std::string Wallet::exportJSON() const {
    std::stringstream ss;
    ss << "{\n"
       << "  \"address\": \""    << address             << "\",\n"
       << "  \"publicKey\": \""  << keyPair.publicKey   << "\",\n"
       << "  \"privateKey\": \"" << keyPair.privateKey  << "\"\n"
       << "}";
    return ss.str();
}

void Wallet::print() const {
    std::cout << "\n╔══════════════════════════════════════════════╗\n";
    std::cout << "║         REX WALLET (Real Crypto)             ║\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    std::cout << "║  Address : " << address             << "\n";
    std::cout << "║  PubKey  : " << keyPair.publicKey.substr(0,32) << "...\n";
    std::cout << "║  Balance : " << balance/100000000.0 << " RXC\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
}
