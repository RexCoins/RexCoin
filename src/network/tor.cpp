#include "tor.h"
#include <iostream>
bool TorSupport::init(const std::string& proxy) {
    // Real impl: connect to Tor control port, request hidden service
    std::cout << "[Tor] Connecting via " << proxy << " (stub)\n";
    enabled   = false; // Set true when Tor is available
    onionAddr = "";
    return false;
}
void TorSupport::stop() { enabled = false; }
