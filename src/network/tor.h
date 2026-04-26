#pragma once
#include <string>
// Tor hidden service support — routes P2P traffic through Tor
// for enhanced node privacy (like Bitcoin Core's -onion option)
class TorSupport {
public:
    bool     init(const std::string& torSocksProxy = "127.0.0.1:9050");
    bool     isEnabled() const { return enabled; }
    std::string getOnionAddress() const { return onionAddr; }
    void     stop();
private:
    bool        enabled   = false;
    std::string onionAddr;
};
