#include "channel.h"
#include "../crypto/sha256.h"
#include <iostream>

std::string PaymentChannel::generateChannelId(const std::string& a,
                                               const std::string& b) {
    return SHA256::hash(a + b + std::to_string(
        channels.size())).substr(0, 16);
}

std::string PaymentChannel::openChannel(const std::string& partyA,
                                         const std::string& partyB,
                                         uint64_t amountA,
                                         uint64_t amountB) {
    Channel ch;
    ch.channelId   = generateChannelId(partyA, partyB);
    ch.partyA      = partyA;
    ch.partyB      = partyB;
    ch.balanceA    = amountA;
    ch.balanceB    = amountB;
    ch.totalLocked = amountA + amountB;
    ch.sequenceNum = 0;
    ch.state       = ChannelState::ACTIVE;
    ch.latestStateHash = SHA256::hash(partyA + partyB +
                                       std::to_string(amountA) +
                                       std::to_string(amountB));
    channels[ch.channelId] = ch;

    std::cout << "\033[32m[Channel] Opened: " << ch.channelId
              << "\n  " << partyA.substr(0,16) << " (" << amountA/1e8 << " RXC)"
              << " ⟷ "
              << partyB.substr(0,16) << " (" << amountB/1e8 << " RXC)"
              << "\033[0m\n";
    return ch.channelId;
}

bool PaymentChannel::pay(const std::string& channelId,
                          const std::string& sender,
                          uint64_t amount) {
    auto it = channels.find(channelId);
    if (it == channels.end()) return false;
    Channel& ch = it->second;
    if (ch.state != ChannelState::ACTIVE) return false;

    // Determine direction
    if (sender == ch.partyA && ch.balanceA >= amount) {
        ch.balanceA -= amount;
        ch.balanceB += amount;
    } else if (sender == ch.partyB && ch.balanceB >= amount) {
        ch.balanceB -= amount;
        ch.balanceA += amount;
    } else {
        std::cout << "\033[31m[Channel] Insufficient balance\033[0m\n";
        return false;
    }

    ch.sequenceNum++;
    ch.latestStateHash = SHA256::hash(ch.latestStateHash +
                                       std::to_string(amount));

    std::cout << "\033[32m[Channel] Off-chain payment: "
              << amount/1e8 << " RXC  (seq=" << ch.sequenceNum << ")\033[0m\n";
    return true;
}

bool PaymentChannel::closeChannel(const std::string& channelId) {
    auto it = channels.find(channelId);
    if (it == channels.end()) return false;
    Channel& ch = it->second;
    ch.state = ChannelState::CLOSED;

    std::cout << "\033[33m[Channel] Closed: " << channelId
              << "\n  Final: A=" << ch.balanceA/1e8
              << " RXC  B=" << ch.balanceB/1e8 << " RXC\033[0m\n";
    return true;
}

const Channel* PaymentChannel::getChannel(const std::string& id) const {
    auto it = channels.find(id);
    return (it != channels.end()) ? &it->second : nullptr;
}

std::vector<std::string> PaymentChannel::getChannels(
    const std::string& address) const
{
    std::vector<std::string> result;
    for (const auto& [id, ch] : channels)
        if (ch.partyA == address || ch.partyB == address)
            result.push_back(id);
    return result;
}

void PaymentChannel::printChannel(const std::string& channelId) const {
    auto* ch = getChannel(channelId);
    if (!ch) { std::cout << "Channel not found\n"; return; }
    std::cout << "\n=== Channel " << channelId << " ===\n";
    std::cout << "  Party A  : " << ch->partyA.substr(0,20)
              << "  bal=" << ch->balanceA/1e8 << " RXC\n";
    std::cout << "  Party B  : " << ch->partyB.substr(0,20)
              << "  bal=" << ch->balanceB/1e8 << " RXC\n";
    std::cout << "  Sequence : " << ch->sequenceNum << "\n";
}
