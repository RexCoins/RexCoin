#pragma once
#include <cstdint>
#include <string>

// RexVM Opcodes — instruction set for smart contracts
enum class Opcode : uint8_t {
    // Stack ops
    PUSH   = 0x01,   // Push value onto stack
    POP    = 0x02,   // Pop top of stack
    DUP    = 0x03,   // Duplicate top
    SWAP   = 0x04,   // Swap top two

    // Arithmetic
    ADD    = 0x10,
    SUB    = 0x11,
    MUL    = 0x12,
    DIV    = 0x13,
    MOD    = 0x14,

    // Comparison
    EQ     = 0x20,
    LT     = 0x21,
    GT     = 0x22,
    AND    = 0x23,
    OR     = 0x24,
    NOT    = 0x25,

    // Crypto
    SHA256 = 0x30,
    HASH   = 0x31,
    VERIFY = 0x32,   // Verify signature

    // Storage
    SSTORE = 0x40,   // Store to contract storage
    SLOAD  = 0x41,   // Load from contract storage

    // Blockchain context
    CALLER    = 0x50, // Who called this contract
    BALANCE   = 0x51, // Get balance
    HEIGHT    = 0x52, // Current block height
    TIMESTAMP = 0x53, // Current block time
    SELFADDR  = 0x54, // This contract's address

    // Control flow
    JUMP   = 0x60,
    JUMPI  = 0x61,   // Conditional jump
    HALT   = 0x62,
    REVERT = 0x63,
    RETURN = 0x64,

    // Token ops (built-in)
    TRANSFER = 0x70, // Transfer RXC
    MINT     = 0x71, // Mint tokens (if authorized)
    BURN     = 0x72, // Burn tokens

    // Events
    LOG    = 0x80,   // Emit an event/log
};

// Get opcode name as string (for debugging)
std::string opcodeName(Opcode op);

// Gas cost per opcode
uint64_t opcodeGasCost(Opcode op);
