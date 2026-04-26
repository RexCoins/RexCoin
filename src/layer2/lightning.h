#pragma once
#include "channel.h"
#include <string>
#include <vector>
#include <unordered_map>

// Lightning-like Network — route payments across multiple channels
// A → B → C even if A and C have no direct channel

struct Route {
    std::vector<std::string> hops;  // node addresses
    uint64_t                 totalFee;
};

class LightningNetwork {
public:
    void   registerNode(const std::string& address);
    void   addChannel(const std::string& channelId,
                      const std::string& nodeA,
                      const std::string& nodeB);
    Route  findRoute(const std::string& from,
                     const std::string& to,
                     uint64_t amount) const;
    bool   sendPayment(const std::string& from,
                       const std::string& to,
                       uint64_t amount);
    void   print() const;
private:
    std::unordered_map<std::string,std::vector<std::string>> graph; // adjacency
    std::unordered_map<std::string,std::string> channelMap;         // edge→channelId
};
