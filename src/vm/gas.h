#pragma once
#include <cstdint>

class GasMeter {
public:
    explicit GasMeter(uint64_t limit) : limit(limit), used(0) {}
    bool consume(uint64_t amount) {
        if (used + amount > limit) return false; // Out of gas
        used += amount;
        return true;
    }
    uint64_t remaining() const { return limit - used; }
    uint64_t getUsed()   const { return used; }
    uint64_t getLimit()  const { return limit; }
    bool     outOfGas()  const { return used >= limit; }
private:
    uint64_t limit;
    uint64_t used;
};
