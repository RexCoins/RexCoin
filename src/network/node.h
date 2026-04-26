#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdint>

// Real P2P Node using POSIX sockets (works on Linux/Termux without Boost)

struct PeerConn {
    int         fd;          // socket file descriptor
    std::string ip;
    uint16_t    port;
    std::string nodeId;
    bool        outbound;    // we connected to them
    int64_t     lastSeen;
    bool        active;
};

using MessageHandler = std::function<void(const std::string& peerId,
                                           const std::string& msgType,
                                           const std::string& payload)>;

class Node {
public:
    Node(const std::string& ip, uint16_t port);
    ~Node();

    // Start listening for connections
    bool start();

    // Stop node
    void stop();

    // Connect to a peer (outbound)
    bool connectToPeer(const std::string& ip, uint16_t port);

    // Broadcast message to all peers
    void broadcast(const std::string& msgType, const std::string& payload);

    // Send to specific peer
    bool sendToPeer(const std::string& peerId,
                    const std::string& msgType,
                    const std::string& payload);

    // Register message handler
    void onMessage(MessageHandler handler);

    // Getters
    bool   isRunning()  const { return running.load(); }
    size_t peerCount()  const;
    std::string getNodeId() const { return nodeId; }
    std::string getIP()     const { return listenIP; }
    uint16_t    getPort()   const { return listenPort; }

    std::vector<PeerConn> getPeers() const;

    void print() const;

private:
    std::string listenIP;
    uint16_t    listenPort;
    std::string nodeId;

    int listenFd = -1;
    std::atomic<bool> running{false};

    mutable std::mutex peersMutex;
    std::unordered_map<std::string, PeerConn> peers;

    std::thread acceptThread;
    std::thread recvThread;

    MessageHandler msgHandler;

    // Internal
    void acceptLoop();
    void recvLoop();
    void handlePeer(PeerConn& peer);
    bool sendRaw(int fd, const std::string& data);
    std::string recvRaw(int fd);
    std::string makePeerId(const std::string& ip, uint16_t port);
    void addPeer(PeerConn peer);
    void removePeer(const std::string& peerId);

    // Message framing: [4-byte length][type:16][payload]
    std::string frame(const std::string& type, const std::string& payload);
    bool        unframe(const std::string& raw, std::string& type, std::string& payload);
};
