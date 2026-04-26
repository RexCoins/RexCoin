#include <iostream>
#include <cassert>
#include "../src/crypto/sha256.h"
#include "../src/crypto/sha3.h"
#include "../src/crypto/ecdsa.h"
#include "../src/crypto/wallet.h"

void test_sha256_vectors() {
    assert(SHA256::hash("") ==
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    assert(SHA256::hash("abc") ==
        "ba7816bf8f01cfea414140de5dae2ec73b00361bbef0469f490f67826a2d6a9");
    std::cout << "[PASS] SHA256 test vectors\n";
}

void test_wallet() {
    Wallet w;
    assert(!w.getAddress().empty());
    assert(!w.getPublicKey().empty());
    assert(!w.getPrivateKey().empty());
    assert(w.getAddress().substr(0,3) == "RXC");
    std::cout << "[PASS] Wallet generation\n";
}

void test_ecdsa() {
    KeyPair kp = ECDSA::generateKeyPair();
    assert(!kp.privateKey.empty());
    assert(!kp.publicKey.empty());
    Signature sig = ECDSA::sign(kp.privateKey, "test_message_hash");
    assert(!sig.r.empty() && !sig.s.empty());
    assert(ECDSA::verify(kp.publicKey, "test_message_hash", sig));
    std::cout << "[PASS] ECDSA sign/verify\n";
}

void test_address() {
    KeyPair kp = ECDSA::generateKeyPair();
    std::string addr = ECDSA::publicKeyToAddress(kp.publicKey);
    assert(addr.substr(0,3) == "RXC");
    assert(addr.size() == 36);
    std::cout << "[PASS] Address generation\n";
}

int main() {
    std::cout << "\n=== RexCoin Crypto Tests ===\n\n";
    test_sha256_vectors();
    test_wallet();
    test_ecdsa();
    test_address();
    std::cout << "\n[ALL TESTS PASSED] ✓\n";
    return 0;
}
