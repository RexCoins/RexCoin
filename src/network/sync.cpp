#include "sync.h"
#include <iostream>
void ChainSync::startSync(const std::string& peer, uint32_t ourHeight) {
    syncing = true;
    std::cout << "[Sync] Starting from peer " << peer
              << " at height " << ourHeight << "\n";
}
void ChainSync::onBlockReceived(const std::string&) {}
