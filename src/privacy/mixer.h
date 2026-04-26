#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Transaction Mixer — breaks linkability between sender and receiver
// Similar to CoinJoin in Bitcoin but automated

struct MixRequest {
    std::string inputAddress;
    std::string outputAddress;
    uint64_t    amount;
    std::string secret;         // Random secret for verification
};

struct MixRound {
    std::vector<MixRequest> participants;
    std::string             roundId;
    bool                    completed;
};

class TxMixer {
public:
    // Min participants for a mix round
    static constexpr int MIN_PARTICIPANTS = 3;

    // Join a mix round
    bool joinMix(const MixRequest& request);

    // Execute the mix when enough participants joined
    bool executeMix(const std::string& roundId);

    // Get current round info
    const MixRound* getCurrentRound() const;

    // Check if a mix is ready
    bool isReady() const;

    void printRound() const;

private:
    MixRound currentRound;
    std::vector<MixRound> completedRounds;
    std::string generateRoundId();
};
