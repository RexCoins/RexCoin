#pragma once
#include <string>
#include "ecdsa.h"

struct WalletInfo {
    std::string address;
    std::string publicKey;
    std::string privateKey;
    uint64_t    balance;
};

class Wallet {
public:
    explicit Wallet();
    explicit Wallet(const std::string& privKey);

    std::string getAddress()    const;
    std::string getPublicKey()  const;
    std::string getPrivateKey() const;
    uint64_t    getBalance()    const;

    Signature signTransaction(const std::string& txHash) const;
    bool      verifySignature(const std::string& txHash,
                               const Signature& sig) const;
    void      setBalance(uint64_t bal);
    std::string exportJSON() const;
    void      print() const;

private:
    KeyPair     keyPair;
    std::string address;
    uint64_t    balance;
};
