#pragma once
#include <string>

// Stealth Addresses — one-time addresses for privacy
// Sender generates a unique address per TX so nobody can link payments
// Based on Monero's stealth address scheme

struct StealthAddress {
    std::string spendPubKey;   // Recipient's spend public key
    std::string viewPubKey;    // Recipient's view public key
};

struct OneTimeAddress {
    std::string address;       // Generated one-time address
    std::string txPubKey;      // Ephemeral public key (included in TX)
};

class StealthAddressSystem {
public:
    // Generate a stealth address pair (done by recipient, shared publicly)
    static StealthAddress generateStealthAddress(const std::string& spendPrivKey,
                                                  const std::string& viewPrivKey);

    // Sender: create a one-time address for this specific payment
    static OneTimeAddress createOneTimeAddress(const StealthAddress& recipient);

    // Recipient: scan TX to check if it belongs to them
    static bool scanTransaction(const std::string& viewPrivKey,
                                 const std::string& txPubKey,
                                 const std::string& outputAddress);

    // Recipient: derive private key to spend the output
    static std::string deriveSpendKey(const std::string& spendPrivKey,
                                       const std::string& viewPrivKey,
                                       const std::string& txPubKey);
};
