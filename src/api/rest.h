#pragma once
#include <string>
#include <unordered_map>
#include <functional>

// REST API — HTTP endpoints for block explorer and wallets
struct HttpRequest  { std::string method, path, body; };
struct HttpResponse { int code; std::string body; };

class RestServer {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;
    explicit RestServer(uint16_t port = 8080);
    void registerRoute(const std::string& method,
                       const std::string& path, Handler h);
    HttpResponse handle(const HttpRequest& req);
    bool start();
    void stop();
    void registerDefaultRoutes();
private:
    uint16_t port; bool running;
    std::unordered_map<std::string,Handler> routes;
};
