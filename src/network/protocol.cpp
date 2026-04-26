#include "protocol.h"
#include "../crypto/sha256.h"
#include <sstream>

std::string Message::serialize() const {
    std::stringstream ss;
    ss << magic << (int)type << length << payload;
    return ss.str();
}

bool Message::isValid() const {
    std::string computed = SHA256::hash(SHA256::hash(payload)).substr(0, 8);
    return computed == checksum;
}

static Message makeMsg(MessageType t, const std::string& payload) {
    Message m;
    m.type     = t;
    m.payload  = payload;
    m.length   = (uint32_t)payload.size();
    m.checksum = SHA256::hash(SHA256::hash(payload)).substr(0, 8);
    return m;
}

Message Protocol::makeVersion(uint32_t blockHeight) {
    return makeMsg(MessageType::VERSION,
        "version=1,height=" + std::to_string(blockHeight) + ",agent=RexCoin/0.1");
}
Message Protocol::makePing()  { return makeMsg(MessageType::PING,  "ping"); }
Message Protocol::makePong(const Message&) { return makeMsg(MessageType::PONG, "pong"); }
Message Protocol::makeBlock(const std::string& d)    { return makeMsg(MessageType::BLOCK, d); }
Message Protocol::makeTx(const std::string& d)       { return makeMsg(MessageType::TX, d); }
Message Protocol::makeGetBlocks(uint32_t h) {
    return makeMsg(MessageType::GETBLOCKS, std::to_string(h));
}
