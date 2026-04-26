#pragma once
#include <string>
#include <unordered_map>
#include <functional>

// JSON-RPC API — same interface as Bitcoin Core
// Allows wallets and explorers to talk to the node

struct RpcRequest {
    std::string method;
    std::string params;   // JSON string
    int         id;
};

struct RpcResponse {
    std::string result;   // JSON string
    std::string error;
    int         id;
    bool        success;
};

class RpcServer {
public:
    using Handler = std::function<RpcResponse(const RpcRequest&)>;

    RpcServer(uint16_t port = 8332);

    // Register a method handler
    void registerMethod(const std::string& method, Handler handler);

    // Handle a raw JSON-RPC request string
    RpcResponse handle(const std::string& rawRequest);

    // Start listening (real impl: TCP server)
    bool start();
    void stop();

    // Built-in RexCoin RPC methods
    void registerDefaultMethods();

    void printMethods() const;

public:
    uint16_t port;
    bool     running;
    std::unordered_map<std::string, Handler> methods;

    RpcRequest  parseRequest(const std::string& raw);
    std::string formatResponse(const RpcResponse& resp);
};
