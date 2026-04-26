#pragma once
#include <string>
#include <cstdint>

// Chain Sync — downloads missing blocks from peers
class ChainSync {
public:
    void startSync(const std::string& peerNodeId, uint32_t ourHeight);
    void onBlockReceived(const std::string& blockData);
    bool isSyncing() const { return syncing; }
private:
    bool     syncing = false;
    uint32_t targetHeight = 0;
};
