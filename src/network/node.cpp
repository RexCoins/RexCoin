#include "node.h"
#include "../crypto/sha256.h"

// POSIX socket headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <algorithm>

// ── Helpers ──────────────────────────────────────────────────────────────────

std::string Node::makePeerId(const std::string& ip, uint16_t port) {
    return SHA256::hash(ip+":"+std::to_string(port)).substr(0,16);
}

std::string Node::frame(const std::string& type, const std::string& payload) {
    // Format: [4-byte LE length][16-byte type padded][payload]
    std::string typeFixed = type;
    typeFixed.resize(16,' ');
    std::string body = typeFixed + payload;
    uint32_t len = (uint32_t)body.size();
    std::string result(4,' ');
    result[0]=(len)&0xFF; result[1]=(len>>8)&0xFF;
    result[2]=(len>>16)&0xFF; result[3]=(len>>24)&0xFF;
    return result + body;
}

bool Node::unframe(const std::string& raw,
                    std::string& type, std::string& payload) {
    if (raw.size() < 20) return false;
    uint32_t len = (uint8_t)raw[0]|(uint8_t)raw[1]<<8|
                   (uint8_t)raw[2]<<16|(uint8_t)raw[3]<<24;
    if (raw.size() < 4+len) return false;
    type    = raw.substr(4,16);
    // Trim trailing spaces from type
    while (!type.empty() && type.back()==' ') type.pop_back();
    payload = raw.substr(20, len-16);
    return true;
}

bool Node::sendRaw(int fd, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = ::send(fd, data.data()+sent, data.size()-sent, MSG_NOSIGNAL);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

std::string Node::recvRaw(int fd) {
    // Read 4-byte length header
    uint8_t hdr[4];
    ssize_t n = ::recv(fd, hdr, 4, MSG_WAITALL);
    if (n != 4) return "";
    uint32_t len = hdr[0]|(hdr[1]<<8)|(hdr[2]<<16)|(hdr[3]<<24);
    if (len > 4*1024*1024) return ""; // 4MB max message
    std::string body(len, '\0');
    n = ::recv(fd, &body[0], len, MSG_WAITALL);
    if (n != (ssize_t)len) return "";
    // Reconstruct with header for unframe()
    std::string raw(4,'\0');
    raw[0]=hdr[0];raw[1]=hdr[1];raw[2]=hdr[2];raw[3]=hdr[3];
    return raw + body;
}

// ── Constructor ───────────────────────────────────────────────────────────────
Node::Node(const std::string& ip, uint16_t port)
    : listenIP(ip), listenPort(port), running(false)
{
    nodeId = makePeerId(ip, std::to_string(port));
}

Node::~Node() { stop(); }

// ── Start ─────────────────────────────────────────────────────────────────────
bool Node::start() {
    listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        std::cerr << "\033[31m[P2P] socket() failed: " << strerror(errno) << "\033[0m\n";
        return false;
    }

    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(listenPort);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(listenFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "\033[31m[P2P] bind() failed: " << strerror(errno) << "\033[0m\n";
        ::close(listenFd);
        return false;
    }

    if (::listen(listenFd, 32) < 0) {
        std::cerr << "\033[31m[P2P] listen() failed\033[0m\n";
        ::close(listenFd);
        return false;
    }

    running = true;
    acceptThread = std::thread(&Node::acceptLoop, this);

    std::cout << "\033[32m[P2P] Node listening on "
              << listenIP << ":" << listenPort
              << "  id=" << nodeId << "\033[0m\n";
    return true;
}

// ── Stop ──────────────────────────────────────────────────────────────────────
void Node::stop() {
    if (!running.load()) return;
    running = false;
    if (listenFd >= 0) { ::shutdown(listenFd, SHUT_RDWR); ::close(listenFd); listenFd=-1; }
    if (acceptThread.joinable()) acceptThread.join();

    std::lock_guard<std::mutex> lock(peersMutex);
    for (auto& [id,p] : peers)
        if (p.fd>=0) ::close(p.fd);
    peers.clear();
    std::cout << "\033[33m[P2P] Node stopped\033[0m\n";
}

// ── Accept Loop ───────────────────────────────────────────────────────────────
void Node::acceptLoop() {
    while (running.load()) {
        struct sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int clientFd = ::accept(listenFd, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientFd < 0) {
            if (running.load())
                std::cerr << "\033[31m[P2P] accept() error\033[0m\n";
            continue;
        }

        char ipBuf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipBuf, sizeof(ipBuf));
        uint16_t remotePort = ntohs(clientAddr.sin_port);

        PeerConn peer;
        peer.fd       = clientFd;
        peer.ip       = std::string(ipBuf);
        peer.port     = remotePort;
        peer.nodeId   = makePeerId(peer.ip, remotePort);
        peer.outbound = false;
        peer.active   = true;
        peer.lastSeen = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::cout << "\033[32m[P2P] Inbound connection from "
                  << peer.ip << ":" << peer.port << "\033[0m\n";

        addPeer(peer);

        // Send VERSION handshake
        sendToPeer(peer.nodeId, "VERSION",
            "version=1,agent=RexCoin/0.2,nodeId=" + nodeId);

        // Handle in separate thread
        std::thread([this, pid=peer.nodeId]() {
            std::lock_guard<std::mutex> lk(peersMutex);
            auto it = peers.find(pid);
            if (it != peers.end()) handlePeer(it->second);
        }).detach();
    }
}

// ── Handle Peer ───────────────────────────────────────────────────────────────
void Node::handlePeer(PeerConn& peer) {
    while (running.load() && peer.active) {
        std::string raw = recvRaw(peer.fd);
        if (raw.empty()) {
            std::cout << "\033[33m[P2P] Peer disconnected: "
                      << peer.ip << "\033[0m\n";
            peer.active = false;
            removePeer(peer.nodeId);
            break;
        }

        std::string type, payload;
        if (!unframe(raw, type, payload)) continue;

        peer.lastSeen = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        // Built-in handlers
        if (type == "PING") {
            sendToPeer(peer.nodeId, "PONG", payload);
        } else if (type == "VERSION") {
            sendToPeer(peer.nodeId, "VERACK", "");
            std::cout << "\033[36m[P2P] Handshake with "
                      << peer.ip << ": " << payload << "\033[0m\n";
        } else if (type == "GETADDR") {
            // Send our peer list
            std::string addrList;
            std::lock_guard<std::mutex> lk(peersMutex);
            for (const auto& [id,p] : peers)
                addrList += p.ip + ":" + std::to_string(p.port) + ",";
            sendToPeer(peer.nodeId, "ADDR", addrList);
        }

        // Call user handler
        if (msgHandler) msgHandler(peer.nodeId, type, payload);
    }
}

// ── Connect to Peer ───────────────────────────────────────────────────────────
bool Node::connectToPeer(const std::string& ip, uint16_t port) {
    std::string pid = makePeerId(ip, port);
    {
        std::lock_guard<std::mutex> lk(peersMutex);
        if (peers.count(pid)) return true; // already connected
    }

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;

    // Set connect timeout (5s)
    struct timeval tv{5,0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    // Resolve hostname
    struct hostent* he = gethostbyname(ip.c_str());
    if (!he) {
        ::close(fd);
        std::cerr << "\033[31m[P2P] Cannot resolve: " << ip << "\033[0m\n";
        return false;
    }
    memcpy(&addr.sin_addr, he->h_addr, he->h_length);

    if (::connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(fd);
        std::cerr << "\033[31m[P2P] Cannot connect to "
                  << ip << ":" << port << "\033[0m\n";
        return false;
    }

    PeerConn peer;
    peer.fd=fd; peer.ip=ip; peer.port=port;
    peer.nodeId=pid; peer.outbound=true;
    peer.active=true;
    peer.lastSeen=std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    addPeer(peer);

    // Handshake
    sendToPeer(pid, "VERSION",
        "version=1,agent=RexCoin/0.2,nodeId="+nodeId);

    // Receive loop in background
    std::thread([this, pid]() {
        std::lock_guard<std::mutex> lk(peersMutex);
        auto it = peers.find(pid);
        if (it != peers.end()) handlePeer(it->second);
    }).detach();

    std::cout << "\033[32m[P2P] Connected to "
              << ip << ":" << port << "\033[0m\n";
    return true;
}

// ── Broadcast ─────────────────────────────────────────────────────────────────
void Node::broadcast(const std::string& type, const std::string& payload) {
    std::string msg = frame(type, payload);
    std::lock_guard<std::mutex> lk(peersMutex);
    int sent = 0;
    for (auto& [id,p] : peers) {
        if (p.active && sendRaw(p.fd, msg)) sent++;
    }
    std::cout << "\033[36m[P2P] Broadcast '" << type
              << "' to " << sent << " peers\033[0m\n";
}

// ── Send to Peer ──────────────────────────────────────────────────────────────
bool Node::sendToPeer(const std::string& peerId,
                       const std::string& type,
                       const std::string& payload) {
    std::lock_guard<std::mutex> lk(peersMutex);
    auto it = peers.find(peerId);
    if (it == peers.end() || !it->second.active) return false;
    return sendRaw(it->second.fd, frame(type, payload));
}

// ── Peer management ───────────────────────────────────────────────────────────
void Node::addPeer(PeerConn peer) {
    std::lock_guard<std::mutex> lk(peersMutex);
    peers[peer.nodeId] = peer;
    std::cout << "\033[32m[P2P] Peers: " << peers.size() << "\033[0m\n";
}

void Node::removePeer(const std::string& peerId) {
    std::lock_guard<std::mutex> lk(peersMutex);
    auto it = peers.find(peerId);
    if (it != peers.end()) {
        if (it->second.fd>=0) ::close(it->second.fd);
        peers.erase(it);
    }
}

void Node::onMessage(MessageHandler handler) { msgHandler = handler; }

size_t Node::peerCount() const {
    std::lock_guard<std::mutex> lk(peersMutex);
    return peers.size();
}

std::vector<PeerConn> Node::getPeers() const {
    std::lock_guard<std::mutex> lk(peersMutex);
    std::vector<PeerConn> result;
    for (const auto& [id,p] : peers) result.push_back(p);
    return result;
}

void Node::print() const {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║  P2P NODE                                ║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    std::cout << "║  ID      : " << nodeId << "\n";
    std::cout << "║  Address : " << listenIP << ":" << listenPort << "\n";
    std::cout << "║  Peers   : " << peerCount() << "\n";
    std::cout << "║  Status  : " << (running.load() ? "🟢 RUNNING" : "🔴 STOPPED") << "\n";
    std::cout << "╚══════════════════════════════════════════╝\n";
    for (const auto& p : getPeers())
        std::cout << "  → " << p.ip << ":" << p.port
                  << (p.outbound ? " [out]" : " [in]") << "\n";
}
