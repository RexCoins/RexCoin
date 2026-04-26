#include "lightning.h"
#include <iostream>
#include <queue>

void LightningNetwork::registerNode(const std::string& addr) {
    if (graph.find(addr) == graph.end()) graph[addr] = {};
}

void LightningNetwork::addChannel(const std::string& channelId,
                                   const std::string& nodeA,
                                   const std::string& nodeB) {
    graph[nodeA].push_back(nodeB);
    graph[nodeB].push_back(nodeA);
    channelMap[nodeA + nodeB] = channelId;
    channelMap[nodeB + nodeA] = channelId;
}

Route LightningNetwork::findRoute(const std::string& from,
                                   const std::string& to,
                                   uint64_t) const {
    // BFS shortest path
    std::unordered_map<std::string,std::string> parent;
    std::queue<std::string> q;
    q.push(from); parent[from] = "";

    while (!q.empty()) {
        auto cur = q.front(); q.pop();
        if (cur == to) break;
        auto it = graph.find(cur);
        if (it == graph.end()) continue;
        for (const auto& nb : it->second)
            if (parent.find(nb) == parent.end()) {
                parent[nb] = cur; q.push(nb);
            }
    }

    Route route;
    if (parent.find(to) == parent.end()) return route; // no path

    for (std::string cur = to; !cur.empty(); cur = parent[cur])
        route.hops.insert(route.hops.begin(), cur);
    route.totalFee = route.hops.size() > 2 ? (route.hops.size()-2) * 10 : 0;
    return route;
}

bool LightningNetwork::sendPayment(const std::string& from,
                                    const std::string& to,
                                    uint64_t amount) {
    Route r = findRoute(from, to, amount);
    if (r.hops.empty()) {
        std::cout << "\033[31m[Lightning] No route found\033[0m\n";
        return false;
    }
    std::cout << "\033[32m[Lightning] Route: ";
    for (const auto& h : r.hops) std::cout << h.substr(0,8) << " → ";
    std::cout << "\n  amount=" << amount/1e8 << " RXC"
              << "  fee=" << r.totalFee << " sat\033[0m\n";
    return true;
}

void LightningNetwork::print() const {
    std::cout << "\n=== Lightning Network ===\n";
    std::cout << "  Nodes: " << graph.size() << "\n";
    for (const auto& [node, peers] : graph)
        std::cout << "  " << node.substr(0,12) << " → "
                  << peers.size() << " peers\n";
}
