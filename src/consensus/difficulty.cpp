#include "difficulty.h"
#include <iostream>
#include <algorithm>

uint32_t DifficultyAdjuster::calculate(uint32_t currentDifficulty,
                                        int64_t  firstBlockTime,
                                        int64_t  lastBlockTime) {
    int64_t elapsed  = lastBlockTime - firstBlockTime;
    int64_t expected = (int64_t)TARGET_BLOCK_TIME * ADJUSTMENT_INTERVAL;

    // Clamp to 4x max change (Bitcoin rule)
    if (elapsed < expected / MAX_ADJUSTMENT) elapsed = expected / MAX_ADJUSTMENT;
    if (elapsed > expected * MAX_ADJUSTMENT) elapsed = expected * MAX_ADJUSTMENT;

    // New difficulty = current * (expected / actual)
    // We simulate with integer: increase/decrease by 1
    uint32_t newDiff = currentDifficulty;
    if (elapsed < expected)
        newDiff = std::min(currentDifficulty + 1, (uint32_t)32);
    else if (elapsed > expected && currentDifficulty > 1)
        newDiff = currentDifficulty - 1;

    std::cout << "\033[33m[Difficulty] " << currentDifficulty
              << " → " << newDiff
              << "  (elapsed=" << elapsed << "s"
              << "  expected=" << expected << "s)\033[0m\n";
    return newDiff;
}

bool DifficultyAdjuster::needsAdjustment(uint32_t blockHeight) {
    return blockHeight > 0 && blockHeight % ADJUSTMENT_INTERVAL == 0;
}

uint32_t DifficultyAdjuster::nextAdjustmentBlock(uint32_t currentHeight) {
    return ((currentHeight / ADJUSTMENT_INTERVAL) + 1) * ADJUSTMENT_INTERVAL;
}
