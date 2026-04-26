# RexCoin Mining Guide

## How Mining Works

RexCoin uses SHA-256 Proof of Work — identical to Bitcoin.

```
1. Collect pending transactions from mempool
2. Build a block with those transactions
3. Try different nonce values until:
   SHA256(SHA256(blockHeader + nonce)) starts with N zeros
4. Broadcast the mined block to network
5. Receive block reward (50 RXC → halves every 210,000 blocks)
```

## Current Reward Schedule

| Block Height  | Reward      |
|--------------|-------------|
| 0 – 209,999  | 50 RXC      |
| 210,000+     | 25 RXC      |
| 420,000+     | 12.5 RXC    |
| 630,000+     | 6.25 RXC    |

## Running a Miner

### Build from source

```bash
git clone https://github.com/yourusername/rexcoin
cd rexcoin
mkdir build && cd build
cmake ..
cmake --build . -j4
```

### Start mining

```bash
./rexcoin --mine --address YOUR_RXC_ADDRESS
```

### With Docker

```bash
docker run rexcoin --mine --address YOUR_RXC_ADDRESS
```

## Difficulty

- Adjusts every **2,016 blocks** (like Bitcoin)
- Target: **1 block per 10 minutes**
- If blocks come faster → difficulty increases
- If blocks come slower → difficulty decreases

## Pool Mining (Coming Soon)

Solo mining may be difficult on testnet.
Mining pools will be supported in a future release.
