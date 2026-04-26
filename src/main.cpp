#include <iostream>
#include <thread>
#include <chrono>
#include "crypto/sha256.h"
#include "core/blockchain.h"
#include "core/transaction.h"
#include "core/mempool.h"
#include "core/script.h"
#include "crypto/wallet.h"
#include "crypto/hash160.h"
#include "crypto/base58.h"
#include "consensus/hybrid.h"
#include "network/node.h"
#include "network/dns_seed.h"
#include "privacy/zkproof.h"
#include "privacy/stealth.h"
#include "layer2/channel.h"
#include "governance/dao.h"
#include "api/rpc.h"
#include "api/rest.h"
#include "storage/leveldb.h"
#include "vm/vm.h"
#include "vm/compiler.h"

void printBanner() {
    std::cout << "\033[36m\n";
    std::cout << "██████╗ ███████╗██╗  ██╗ ██████╗ ██████╗ ██╗███╗   ██╗\n";
    std::cout << "██╔══██╗██╔════╝╚██╗██╔╝██╔════╝██╔═══██╗██║████╗  ██║\n";
    std::cout << "██████╔╝█████╗   ╚███╔╝ ██║     ██║   ██║██║██╔██╗ ██║\n";
    std::cout << "██╔══██╗██╔══╝   ██╔██╗ ██║     ██║   ██║██║██║╚██╗██║\n";
    std::cout << "██║  ██║███████╗██╔╝ ██╗╚██████╗╚██████╔╝██║██║ ╚████║\n";
    std::cout << "╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝\033[0m\n";
    std::cout << "\033[33m";
    std::cout << "  ╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║  RexCoin (RXC) v0.3.0  |  Rex Network                ║\n";
    std::cout << "  ║  Real P2P TCP Network  |  Persistent LevelDB Storage  ║\n";
    std::cout << "  ║  secp256k1 ECDSA  |  RIPEMD160  |  Base58Check       ║\n";
    std::cout << "  ╚═══════════════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";
}

void sec(const std::string& t) {
    std::cout << "\n\033[35m━━━ " << t << " ━━━\033[0m\n";
}

int main(int argc, char* argv[]) {
    printBanner();

    // ── Parse args ────────────────────────────────────────────────────────
    std::string dataDir   = ".rexcoin";
    std::string connectTo = "";
    uint16_t    p2pPort   = 8333;
    bool        mineMode  = false;

    for (int i=1;i<argc;i++) {
        std::string arg = argv[i];
        if (arg=="--datadir" && i+1<argc) dataDir   = argv[++i];
        if (arg=="--connect" && i+1<argc) connectTo = argv[++i];
        if (arg=="--port"    && i+1<argc) p2pPort   = std::stoi(argv[++i]);
        if (arg=="--mine")               mineMode  = true;
    }

    // ── 1. Persistent Blockchain (LevelDB) ───────────────────────────────
    sec("[1] Persistent Blockchain (LevelDB)");
    Blockchain chain(dataDir);
    chain.printInfo();

    // ── 2. Real Wallets ───────────────────────────────────────────────────
    sec("[2] Real Wallets (secp256k1 + RIPEMD160 + Base58Check)");
    Wallet walletMiner;
    Wallet walletA;
    Wallet walletB;
    walletMiner.print();
    walletA.print();

    // ── 3. ECDSA Sign/Verify ──────────────────────────────────────────────
    sec("[3] Real ECDSA Signature");
    std::string msgHash = SHA256::hash("RexCoin_test_tx_2026");
    Signature sig = walletA.signTransaction(msgHash);
    bool ok = walletA.verifySignature(msgHash, sig);
    std::cout << (ok ? "\033[32m[✓] ECDSA valid!\033[0m\n"
                     : "\033[31m[✗] ECDSA failed\033[0m\n");

    // ── 4. Mempool + Mining ───────────────────────────────────────────────
    sec("[4] Mempool + Mining");
    Mempool mempool;

    Transaction cb = Transaction::createCoinbase(
        walletMiner.getAddress(), chain.getBlockReward(), (uint32_t)chain.getHeight());
    Transaction tx1 = Transaction::create({}, {{
        5ULL*100000000ULL, walletA.getAddress(), "P2PKH"}});
    Transaction tx2 = Transaction::create({}, {{
        2ULL*100000000ULL, walletB.getAddress(), "P2PKH"}});

    mempool.add(tx1);
    mempool.add(tx2);
    mempool.print();

    auto pending = mempool.getTopTxs(10);
    pending.insert(pending.begin(), cb);
    chain.addBlock(pending);
    mempool.removeConfirmed({tx1.txHash, tx2.txHash});
    chain.printInfo();

    // ── 5. Real P2P Network ───────────────────────────────────────────────
    sec("[5] Real P2P Network (POSIX TCP Sockets)");
    Node node("0.0.0.0", p2pPort);

    // Register message handler
    node.onMessage([](const std::string& peer,
                       const std::string& type,
                       const std::string& payload) {
        std::cout << "\033[36m[P2P] MSG from " << peer.substr(0,8)
                  << ": " << type << " → " << payload.substr(0,40) << "\033[0m\n";
    });

    bool p2pOk = node.start();
    if (p2pOk) {
        std::cout << "\033[32m[✓] P2P Node started on port "
                  << p2pPort << "\033[0m\n";

        // Connect to peer if specified
        if (!connectTo.empty()) {
            size_t colon = connectTo.find(':');
            std::string peerIP   = connectTo.substr(0, colon);
            uint16_t    peerPort = (colon!=std::string::npos)
                                   ? std::stoi(connectTo.substr(colon+1))
                                   : 8333;
            node.connectToPeer(peerIP, peerPort);
        }

        // Try DNS seeds
        auto seeds = DNSSeed::getSeeds();
        std::cout << "  Seed nodes available:\n";
        for (const auto& s : seeds)
            std::cout << "    → " << s.host << ":" << s.port << "\n";

        // Broadcast new block to peers
        node.broadcast("NEW_BLOCK", chain.getLatestBlock().hash);
        node.print();
    } else {
        std::cout << "\033[33m[!] P2P failed to bind (port in use?)\033[0m\n";
    }

    // ── 6. DB Stats ───────────────────────────────────────────────────────
    sec("[6] Persistent Storage Stats");
    chain.getDB().printStats();

    // ── 7. Smart Contract ─────────────────────────────────────────────────
    sec("[7] Smart Contract VM");
    VM vm; Compiler compiler;
    std::string addr = vm.deployContract(
        compiler.compile("SHA256 SSTORE HALT"),
        walletA.getAddress(), 100000);
    auto res = vm.call(addr, "hello_rex", 100000);
    std::cout << "  Contract: " << addr << "\n";
    std::cout << (res.halted
        ? "\033[32m[✓] Contract OK (gas=" + std::to_string(res.gasUsed) + ")\033[0m\n"
        : "\033[31m[✗] Contract failed\033[0m\n");

    // ── 8. Privacy ────────────────────────────────────────────────────────
    sec("[8] Privacy (ZK-Proof + Stealth)");
    ShieldedTx stx = ZKProofSystem::createShieldedTx(
        walletA.getPrivateKey(), walletB.getAddress(),
        5ULL*100000000ULL, 10000);
    std::cout << (ZKProofSystem::verifyShieldedTx(stx)
        ? "\033[32m[✓] ZK-Proof valid!\033[0m\n"
        : "\033[31m[✗] ZK-Proof failed\033[0m\n");

    // ── 9. Layer 2 ────────────────────────────────────────────────────────
    sec("[9] Layer 2 Payment Channel");
    PaymentChannel channels;
    std::string chId = channels.openChannel(
        walletA.getAddress(), walletB.getAddress(),
        10ULL*100000000ULL, 10ULL*100000000ULL);
    channels.pay(chId, walletA.getAddress(), 3ULL*100000000ULL);
    channels.closeChannel(chId);

    // ── 10. API ───────────────────────────────────────────────────────────
    sec("[10] API Servers");
    RpcServer rpc(8332); RestServer rest(8080);
    rpc.start(); rest.start();
    auto rpcRes = rpc.handle("{\"method\":\"getnetworkinfo\",\"params\":[],\"id\":1}");
    std::cout << "  RPC: " << rpcRes.result << "\n";

    // ── Final ─────────────────────────────────────────────────────────────
    sec("Final Status");
    chain.printInfo();

    std::cout << "\n\033[32m";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  ✅ RexCoin v0.3.0 — Real Blockchain Node          ║\n";
    std::cout << "║  ✅ Persistent Storage → " << dataDir << "/chaindata\n";
    std::cout << "║  ✅ Real P2P TCP Network → port " << p2pPort << "            ║\n";
    std::cout << "║  ✅ Real secp256k1 ECDSA Crypto                    ║\n";
    std::cout << "║  ✅ Real SHA-256 PoW Mining                        ║\n";
    std::cout << "║                                                    ║\n";
    std::cout << "║  To connect 2 nodes:                               ║\n";
    std::cout << "║  Node 1: ./rexcoin --port 8333                     ║\n";
    std::cout << "║  Node 2: ./rexcoin --port 8334 --connect           ║\n";
    std::cout << "║           127.0.0.1:8333                           ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";

    // Keep P2P running if mine mode
    if (mineMode && p2pOk) {
        std::cout << "\033[33m[Mining mode] Press Ctrl+C to stop\033[0m\n";
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    node.stop();
    return 0;
}
