#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

enum class ChannelState { OPEN, ACTIVE, CLOSING, CLOSED };

struct Channel {
    std::string  channelId;
    std::string  partyA, partyB;
    uint64_t     balanceA, balanceB, totalLocked;
    uint32_t     sequenceNum;
    ChannelState state;
    std::string  latestStateHash;
};

class PaymentChannel {
public:
    std::string openChannel(const std::string& a, const std::string& b,
                             uint64_t amtA, uint64_t amtB);
    bool pay(const std::string& channelId, const std::string& sender,
             uint64_t amount);
    bool closeChannel(const std::string& channelId);
    const Channel* getChannel(const std::string& id) const;
    std::vector<std::string> getChannels(const std::string& address) const;
    void printChannel(const std::string& channelId) const;
private:
    std::unordered_map<std::string, Channel> channels;
    std::string generateChannelId(const std::string& a, const std::string& b);
};
