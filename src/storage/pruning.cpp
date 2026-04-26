#include "pruning.h"
#include <iostream>
uint32_t ChainPruner::prune(uint32_t currentHeight){
    if(currentHeight<=keepBlocks)return 0;
    uint32_t target=currentHeight-keepBlocks;
    uint32_t pruned=target-prunedHeight;
    prunedHeight=target;
    std::cout<<"[Pruner] Pruned to height "<<prunedHeight<<" ("<<pruned<<" blocks)\n";
    return pruned;
}
