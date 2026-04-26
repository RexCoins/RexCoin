#pragma once
#include <string>
#include <cstdint>

// Rex Network Wire Protocol
// Message types (like Bitcoin's protocol)
enum class MessageType : uint8_t {
    VERSION     = 0x01,
    VERACK      = 0x02,
    PING        = 0x03,
    PONG        = 0x04,
    GETBLOCKS   = 0x05,
    BLOCK       = 0x06,
    TX          = 0x07,
    GETADDR     = 0x08,
    ADDR        = 0x09,
    INV         = 0x0A,
    GETDATA     = 0x0B,
    REJECT      = 0x0C,
};

struct Message {
    uint32_t    magic     = 0x52455843; // "REXC" in hex
    MessageType type;
    uint32_t    length;
    std::string payload;
    std::string checksum; // First 4 bytes of SHA256(SHA256(payload))

    std::string serialize()   const;
    static Message deserialize(const std::string& data);
    bool isValid()            const;
};

class Protocol {
public:
    static Message makeVersion(uint32_t blockHeight);
    static Message makePing();
    static Message makePong(const Message& ping);
    static Message makeBlock(const std::string& blockData);
    static Message makeTx(const std::string& txData);
    static Message makeGetBlocks(uint32_t fromHeight);
};
