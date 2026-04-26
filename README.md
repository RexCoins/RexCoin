# ⚡ RexCoin (RXC)

> A next-generation, Bitcoin-inspired blockchain written in C++17 — built from scratch with advanced features beyond Bitcoin.

```
██████╗ ███████╗██╗  ██╗
██╔══██╗██╔════╝╚██╗██╔╝
██████╔╝█████╗   ╚███╔╝
██╔══██╗██╔══╝   ██╔██╗
██║  ██║███████╗██╔╝ ██╗
╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
   RexCoin (RXC) v0.1.0
```

## 📋 Specifications

| Property | Value |
|----------|-------|
| **Coin Name** | RexCoin |
| **Ticker** | RXC |
| **Total Supply** | 21,000,000 RXC |
| **Block Time** | 10 Minutes |
| **Block Reward** | 50 RXC (halves every 210,000 blocks) |
| **Algorithm** | SHA-256 (PoW) + PoS Hybrid |
| **Network** | Rex Network |
| **Language** | C++17 |

## 🚀 Features

### Bitcoin-Level
- ✅ SHA-256 Proof of Work
- ✅ UTXO Transaction Model
- ✅ ECDSA Digital Signatures
- ✅ Merkle Tree
- ✅ Difficulty Adjustment (every 2016 blocks)
- ✅ Block Reward Halving
- ✅ P2P Network
- ✅ Mempool

### Advanced (Beyond Bitcoin)
- ✅ PoW + PoS Hybrid Consensus
- ✅ Smart Contract VM
- ✅ Zero-Knowledge Proofs (Privacy)
- ✅ Stealth Addresses
- ✅ BLS Signatures
- ✅ Layer 2 Payment Channels
- ✅ Lightning-like Network
- ✅ On-chain Governance (DAO)
- ✅ Tor Network Support
- ✅ Native Token System
- ✅ JSON-RPC + REST API
- ✅ WebSocket Real-time Updates

## 🛠️ Build

### Requirements
- CMake 3.16+
- C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- OpenSSL
- Boost 1.71+

### Linux / macOS
```bash
git clone https://github.com/RexCoins/rexcoin
cd rexcoin
mkdir build && cd build
cmake ..
cmake --build . -j4
./rexcoin
```

### Windows
```bash
git clone https://github.com/RexCoins/rexcoin
cd rexcoin
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
rexcoin.exe
```

## 📁 Project Structure

```
RexCoin/
├── src/
│   ├── core/          # Block, Blockchain, Transaction, UTXO, Merkle
│   ├── consensus/     # PoW, PoS, Hybrid, Difficulty
│   ├── crypto/        # SHA256, SHA3, ECDSA, BLS, Wallet
│   ├── network/       # Node, Peer, Protocol, Sync, Tor
│   ├── vm/            # Smart Contract VM, Opcodes, Gas
│   ├── privacy/       # ZK-Proof, Stealth, Mixer
│   ├── layer2/        # Payment Channel, Lightning, Sidechain
│   ├── governance/    # Proposal, Vote, DAO
│   ├── storage/       # LevelDB, Cache, Pruning
│   └── api/           # RPC, REST, WebSocket
├── tests/
├── docs/
└── docker/
```

## 🧪 Run Tests
```bash
cd build
ctest --verbose
```

## 🐳 Docker
```bash
docker build -t rexcoin .
docker run rexcoin
```

## 📄 License
MIT License — see [LICENSE](LICENSE)

## 🤝 Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md)

## 📖 Whitepaper
See [WHITEPAPER.md](WHITEPAPER.md)

## Contributors
* [Name/Username](https://github.com/username) - Lead Developer
* [Name/Username](https://github.com/username) - UI Design
* 
