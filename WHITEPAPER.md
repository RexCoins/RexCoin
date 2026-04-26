# RexCoin Whitepaper

**Version:** 0.1.0  
**Network:** Rex Network  
**Ticker:** RXC

---

## Abstract

RexCoin (RXC) is a next-generation, open-source blockchain protocol built in C++17.
It inherits Bitcoin's proven economic model — 21 million supply, 10-minute blocks,
SHA-256 PoW, and halving — while extending it with smart contracts, privacy proofs,
Layer-2 payment channels, and on-chain governance.

---

## 1. Specifications

| Property | Value |
|----------|-------|
| Total Supply | 21,000,000 RXC |
| Block Time | 10 minutes |
| Initial Reward | 50 RXC |
| Halving Interval | 210,000 blocks |
| Difficulty Adjust | Every 2,016 blocks |
| Consensus | PoW + PoS Hybrid |
| Hash Algorithm | SHA-256 |
| Signature | ECDSA + BLS |

---

## 2. Consensus — PoW + PoS Hybrid

RexCoin uses a hybrid consensus mechanism:

- **Proof of Work (PoW):** Miners solve SHA-256 puzzles to propose blocks.
- **Proof of Stake (PoS):** Validators holding RXC stake vote to finalize blocks.
- This hybrid approach provides Bitcoin-level security with faster finality.

---

## 3. Transaction Model — UTXO

Like Bitcoin, RexCoin uses the UTXO (Unspent Transaction Output) model.
Every transaction consumes existing UTXOs and creates new ones.
Digital signatures (ECDSA) prove ownership.

---

## 4. Smart Contracts

RexCoin includes a built-in virtual machine (RexVM) supporting:
- Custom opcodes
- Gas-based execution metering
- Token creation
- DeFi primitives

---

## 5. Privacy

- **Zero-Knowledge Proofs:** Shielded transactions hide amounts and addresses.
- **Stealth Addresses:** One-time addresses prevent address reuse tracking.
- **Transaction Mixer:** Breaks transaction graph linkability.

---

## 6. Layer 2

- **Payment Channels:** Off-chain transactions with on-chain settlement.
- **Lightning-like Network:** Routed micropayments across channel network.
- **Sidechains:** Parallel chains for specialized use cases.

---

## 7. Governance — DAO

RXC holders can submit and vote on protocol proposals on-chain.
A supermajority (67%) is required to pass consensus changes.

---

## 8. Supply Schedule

| Halving | Block Height | Reward |
|---------|-------------|--------|
| 0 | 0 | 50 RXC |
| 1 | 210,000 | 25 RXC |
| 2 | 420,000 | 12.5 RXC |
| 3 | 630,000 | 6.25 RXC |
| 4 | 840,000 | 3.125 RXC |
| ... | ... | ... |

---

## 9. Roadmap

- **Phase 1:** Core blockchain + PoW mining
- **Phase 2:** Wallet + UTXO + Transactions
- **Phase 3:** P2P Network
- **Phase 4:** Smart Contract VM
- **Phase 5:** Privacy layer
- **Phase 6:** Layer 2
- **Phase 7:** Governance
- **Phase 8:** Public Testnet
- **Phase 9:** Mainnet Launch 🚀
