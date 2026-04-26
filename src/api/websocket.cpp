#include "websocket.h"
#include <iostream>

WebSocketServer::WebSocketServer(uint16_t p):port(p),running(false){}

bool WebSocketServer::start(){
    running=true;
    std::cout<<"\033[32m[WS] WebSocket started on port "<<port<<"\033[0m\n";
    return true;
}
void WebSocketServer::stop(){ running=false; }

void WebSocketServer::broadcast(WsEvent event, const std::string& data){
    for(auto&[e,cb]:subscribers) if(e==event) cb(data);
    std::cout<<"\033[36m[WS] Broadcast: "<<data.substr(0,60)<<"...\033[0m\n";
}

void WebSocketServer::subscribe(WsEvent event, Callback cb){
    subscribers.push_back({event,cb});
}
