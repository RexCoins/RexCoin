#include "rpc.h"
#include <iostream>
#include <sstream>

RpcServer::RpcServer(uint16_t p) : port(p), running(false) {}

void RpcServer::registerMethod(const std::string& method, Handler handler) {
    methods[method] = handler;
}

RpcRequest RpcServer::parseRequest(const std::string& raw) {
    // Minimal JSON parser for "method" field
    RpcRequest req; req.id = 1;
    size_t mPos = raw.find("\"method\"");
    if (mPos != std::string::npos) {
        size_t start = raw.find('"', mPos + 9) + 1;
        size_t end   = raw.find('"', start);
        req.method   = raw.substr(start, end - start);
    }
    size_t pPos = raw.find("\"params\"");
    if (pPos != std::string::npos) {
        size_t start = raw.find('[', pPos);
        size_t end   = raw.find(']', start) + 1;
        req.params   = raw.substr(start, end - start);
    }
    return req;
}

std::string RpcServer::formatResponse(const RpcResponse& resp) {
    std::stringstream ss;
    ss << "{\"id\":" << resp.id;
    if (resp.success)
        ss << ",\"result\":" << resp.result << ",\"error\":null}";
    else
        ss << ",\"result\":null,\"error\":\"" << resp.error << "\"}";
    return ss.str();
}

RpcResponse RpcServer::handle(const std::string& rawRequest) {
    RpcRequest req = parseRequest(rawRequest);
    auto it = methods.find(req.method);
    if (it == methods.end()) {
        RpcResponse err; err.id = req.id;
        err.success = false;
        err.error   = "Method not found: " + req.method;
        return err;
    }
    return it->second(req);
}

void RpcServer::registerDefaultMethods() {
    registerMethod("getblockcount", [](const RpcRequest& r) {
        RpcResponse resp; resp.id = r.id; resp.success = true;
        resp.result = "0"; // TODO: real block height
        return resp;
    });
    registerMethod("getnetworkinfo", [](const RpcRequest& r) {
        RpcResponse resp; resp.id = r.id; resp.success = true;
        resp.result = "{\"name\":\"Rex Network\",\"version\":\"0.1.0\",\"coin\":\"RXC\"}";
        return resp;
    });
    registerMethod("getbalance", [](const RpcRequest& r) {
        RpcResponse resp; resp.id = r.id; resp.success = true;
        resp.result = "0.0"; // TODO: real balance lookup
        return resp;
    });
    registerMethod("sendtoaddress", [](const RpcRequest& r) {
        RpcResponse resp; resp.id = r.id; resp.success = true;
        resp.result = "\"txhash_placeholder\"";
        return resp;
    });
    registerMethod("getmininginfo", [](const RpcRequest& r) {
        RpcResponse resp; resp.id = r.id; resp.success = true;
        resp.result = "{\"difficulty\":4,\"reward\":50}";
        return resp;
    });
}

bool RpcServer::start() {
    running = true;
    registerDefaultMethods();
    std::cout << "\033[32m[RPC] Server started on port " << port << "\033[0m\n";
    return true;
}

void RpcServer::stop() {
    running = false;
    std::cout << "\033[33m[RPC] Server stopped\033[0m\n";
}

void RpcServer::printMethods() const {
    std::cout << "\n=== RPC Methods ===\n";
    for (const auto& [name, _] : methods)
        std::cout << "  " << name << "\n";
}
