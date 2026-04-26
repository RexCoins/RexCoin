#include "rest.h"
#include <iostream>

RestServer::RestServer(uint16_t p):port(p),running(false){}

void RestServer::registerRoute(const std::string& m,
                                const std::string& p, Handler h){
    routes[m+" "+p]=h;
}

HttpResponse RestServer::handle(const HttpRequest& req){
    auto it=routes.find(req.method+" "+req.path);
    if(it==routes.end())
        return {404,"{\"error\":\"Not found\"}"};
    return it->second(req);
}

void RestServer::registerDefaultRoutes(){
    registerRoute("GET","/blocks",    [](const HttpRequest&){
        return HttpResponse{200,"{\"blocks\":[]}"};});
    registerRoute("GET","/mempool",   [](const HttpRequest&){
        return HttpResponse{200,"{\"count\":0,\"txs\":[]}"};});
    registerRoute("GET","/stats",     [](const HttpRequest&){
        return HttpResponse{200,"{\"coin\":\"RXC\",\"supply\":21000000}"};});
    registerRoute("GET","/tx/{hash}", [](const HttpRequest&){
        return HttpResponse{200,"{\"tx\":{}}"};});
}

bool RestServer::start(){
    running=true; registerDefaultRoutes();
    std::cout<<"\033[32m[REST] API started on port "<<port<<"\033[0m\n";
    return true;
}
void RestServer::stop(){ running=false; }
