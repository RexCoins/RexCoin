#include "peer.h"
#include <algorithm>
#include <chrono>

void PeerManager::addPeer(const std::string& nodeId) {
    PeerScore ps;
    ps.nodeId    = nodeId;
    ps.score     = 100;
    ps.lastSeen  = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    ps.bytesRecv = 0;
    ps.bytesSent = 0;
    peers.push_back(ps);
}

void PeerManager::removePeer(const std::string& nodeId) {
    peers.erase(std::remove_if(peers.begin(), peers.end(),
        [&](const PeerScore& p){ return p.nodeId == nodeId; }), peers.end());
}

void PeerManager::updateScore(const std::string& nodeId, int delta) {
    for (auto& p : peers)
        if (p.nodeId == nodeId) { p.score += delta; break; }
}

std::vector<PeerScore> PeerManager::getBestPeers(int count) const {
    auto sorted = peers;
    std::sort(sorted.begin(), sorted.end(),
        [](const PeerScore& a, const PeerScore& b){ return a.score > b.score; });
    if ((int)sorted.size() > count) sorted.resize(count);
    return sorted;
}

size_t PeerManager::count() const { return peers.size(); }
