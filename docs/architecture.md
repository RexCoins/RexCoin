# RexCoin Architecture

## Overview

RexCoin is a layered blockchain system. Each layer builds on top of the previous.

```
┌─────────────────────────────────────────┐
│           Applications / Wallets         │  ← Users interact here
├─────────────────────────────────────────┤
│     REST API  │  JSON-RPC  │  WebSocket  │  ← api/
├─────────────────────────────────────────┤
│   Governance (DAO)  │  Layer 2 (LN)      │  ← governance/ layer2/
├─────────────────────────────────────────┤
│   Privacy (ZK-Proof, Stealth, Mixer)    │  ← privacy/
├─────────────────────────────────────────┤
│         Smart Contract VM               │  ← vm/
├─────────────────────────────────────────┤
│   P2P Network  │  Protocol  │  Sync     │  ← network/
├─────────────────────────────────────────┤
│   PoW + PoS Hybrid Consensus            │  ← consensus/
├─────────────────────────────────────────┤
│   Block │ Blockchain │ TX │ UTXO        │  ← core/
├─────────────────────────────────────────┤
│   SHA256 │ SHA3 │ ECDSA │ BLS │ Wallet  │  ← crypto/
├─────────────────────────────────────────┤
│         LevelDB Storage + LRU Cache     │  ← storage/
└─────────────────────────────────────────┘
```

## Data Flow — Sending a Transaction

```
User Wallet
    │
    │  sign TX with private key (ECDSA)
    ▼
Transaction Created
    │
    │  broadcast to network
    ▼
P2P Node → Gossip to all peers
    │
    │  validated by each node
    ▼
Mempool (pending TXs)
    │
    │  miner picks TXs
    ▼
Block Mining (PoW — find nonce)
    │
    │  block proposed
    ▼
PoS Validators vote to finalize
    │
    │  2/3 threshold reached
    ▼
Block Added to Chain
    │
    │  UTXO set updated
    ▼
Recipient's wallet sees balance
```

## Block Structure

```
┌───────────────────────────────┐
│         BLOCK HEADER          │
│  version       : 4 bytes      │
│  previousHash  : 32 bytes     │
│  merkleRoot    : 32 bytes     │
│  timestamp     : 8 bytes      │
│  difficulty    : 4 bytes      │
│  nonce         : 8 bytes      │
├───────────────────────────────┤
│         TRANSACTIONS          │
│  coinbase TX (miner reward)   │
│  TX 1                         │
│  TX 2                         │
│  ...                          │
└───────────────────────────────┘
```

## Consensus — PoW + PoS Hybrid

```
Step 1: Miner runs SHA256 PoW
        finds nonce → block hash starts with N zeros

Step 2: Block broadcast to network

Step 3: PoS validators vote
        need 67% stake-weighted votes to finalize

Step 4: Finalized block added to chain
```

## Privacy Architecture

```
Normal TX:   Sender ──────────────────► Receiver
             (addresses visible on-chain)

Shielded TX: Sender ──[ZK Proof]──────► Receiver
             (amounts + addresses hidden)

Stealth TX:  Sender ──[One-time addr]──► Receiver
             (receiver address unlinked)

Mixed TX:    A,B,C ──[CoinJoin]──► A',B',C'
             (input-output link broken)
```
