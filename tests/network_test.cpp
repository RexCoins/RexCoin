#include <iostream>
#include <cassert>
#include "../src/network/node.h"
#include "../src/network/peer.h"
#include "../src/network/protocol.h"

void test_node() {
    Node node("127.0.0.1", 8333);
    assert(!node.isRunning());
    node.start();
    assert(node.isRunning());
    assert(node.peerCount() == 0);
    node.connectToPeer("192.168.1.1", 8333);
    assert(node.peerCount() == 1);
    std::cout << "[PASS] Node start/connect\n";
}

void test_peer_manager() {
    PeerManager pm;
    pm.addPeer("node_abc");
    pm.addPeer("node_def");
    assert(pm.count() == 2);
    pm.updateScore("node_abc", 50);
    auto best = pm.getBestPeers(1);
    assert(best[0].nodeId == "node_abc");
    pm.removePeer("node_abc");
    assert(pm.count() == 1);
    std::cout << "[PASS] PeerManager\n";
}

void test_protocol() {
    Message ping = Protocol::makePing();
    assert((int)ping.type == (int)MessageType::PING);
    Message pong = Protocol::makePong(ping);
    assert((int)pong.type == (int)MessageType::PONG);
    Message ver = Protocol::makeVersion(100);
    assert((int)ver.type == (int)MessageType::VERSION);
    std::cout << "[PASS] Protocol messages\n";
}

int main() {
    std::cout << "\n=== RexCoin Network Tests ===\n\n";
    test_node();
    test_peer_manager();
    test_protocol();
    std::cout << "\n[ALL TESTS PASSED] ✓\n";
    return 0;
}
