#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Peer management — discovery and scoring
struct PeerScore {
    std::string nodeId;
    int         score;     // Higher = more reliable
    uint64_t    lastSeen;
    uint64_t    bytesRecv;
    uint64_t    bytesSent;
};

class PeerManager {
public:
    void   addPeer(const std::string& nodeId);
    void   removePeer(const std::string& nodeId);
    void   updateScore(const std::string& nodeId, int delta);
    std::vector<PeerScore> getBestPeers(int count) const;
    size_t count() const;

private:
    std::vector<PeerScore> peers;
};
