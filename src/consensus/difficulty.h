#pragma once
#include <cstdint>
#include <vector>

// Difficulty Adjustment Algorithm — identical to Bitcoin
// Adjusts every 2016 blocks to target 10-minute block time
class DifficultyAdjuster {
public:
    static constexpr uint32_t ADJUSTMENT_INTERVAL = 2016;
    static constexpr uint32_t TARGET_BLOCK_TIME   = 600; // seconds
    static constexpr uint32_t MAX_ADJUSTMENT      = 4;   // 4x max change

    // Calculate new difficulty
    static uint32_t calculate(uint32_t currentDifficulty,
                               int64_t  firstBlockTime,
                               int64_t  lastBlockTime);

    // Check if adjustment is needed at this height
    static bool needsAdjustment(uint32_t blockHeight);

    // Estimate next adjustment block
    static uint32_t nextAdjustmentBlock(uint32_t currentHeight);
};
