#pragma once
#include <string>
#include <vector>

// DNS Seeds — hardcoded hostnames that return IP addresses of stable nodes
// Bitcoin uses: seed.bitcoin.sipa.be, dnsseed.bluematt.me etc.
// RexCoin uses its own seeds

struct SeedNode {
    std::string host;
    uint16_t    port;
    std::string description;
};

class DNSSeed {
public:
    // Hardcoded RexCoin seed nodes
    static const std::vector<SeedNode> SEEDS;

    // Get initial peers to connect to
    static std::vector<SeedNode> getSeeds();

    // Resolve hostname to IPs (real impl uses getaddrinfo)
    static std::vector<std::string> resolve(const std::string& hostname);

    // Check if a node is reachable (ping)
    static bool isReachable(const std::string& ip, uint16_t port);
};
