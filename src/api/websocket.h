#pragma once
#include <string>
#include <vector>
#include <functional>

// WebSocket API — real-time updates for block explorer and wallets
// Broadcasts new blocks, transactions, and mempool changes live

enum class WsEvent { NEW_BLOCK, NEW_TX, MEMPOOL_UPDATE, PRICE_UPDATE };

class WebSocketServer {
public:
    using Callback = std::function<void(const std::string&)>;
    explicit WebSocketServer(uint16_t port = 8765);
    bool   start();
    void   stop();
    void   broadcast(WsEvent event, const std::string& data);
    void   subscribe(WsEvent event, Callback cb);
    size_t clientCount() const { return clientCount_; }
private:
    uint16_t port; bool running; size_t clientCount_=0;
    std::vector<std::pair<WsEvent,Callback>> subscribers;
};
