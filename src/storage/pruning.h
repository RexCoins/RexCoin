#pragma once
#include <cstdint>
class ChainPruner {
public:
    explicit ChainPruner(uint32_t keepBlocks=1000):keepBlocks(keepBlocks),prunedHeight(0){}
    uint32_t prune(uint32_t currentHeight);
    uint32_t getPrunedHeight()const{return prunedHeight;}
private:
    uint32_t keepBlocks, prunedHeight;
};
