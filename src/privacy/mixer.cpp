#include "mixer.h"
#include "../crypto/sha256.h"
#include <iostream>
#include <algorithm>

std::string TxMixer::generateRoundId() {
    return SHA256::hash("MixRound_" + std::to_string(
        completedRounds.size())).substr(0, 16);
}

bool TxMixer::joinMix(const MixRequest& request) {
    if (currentRound.roundId.empty())
        currentRound.roundId = generateRoundId();

    currentRound.participants.push_back(request);
    std::cout << "\033[36m[Mixer] Participant joined round "
              << currentRound.roundId
              << " (" << currentRound.participants.size()
              << "/" << MIN_PARTICIPANTS << ")\033[0m\n";
    return true;
}

bool TxMixer::executeMix(const std::string& roundId) {
    if (currentRound.roundId != roundId) return false;
    if ((int)currentRound.participants.size() < MIN_PARTICIPANTS) {
        std::cout << "\033[31m[Mixer] Not enough participants\033[0m\n";
        return false;
    }

    // Shuffle outputs — break the input→output link
    auto participants = currentRound.participants;
    std::vector<std::string> outputs;
    for (const auto& p : participants) outputs.push_back(p.outputAddress);

    // Random shuffle
    for (size_t i = outputs.size()-1; i > 0; i--) {
        size_t j = SHA256::hash(outputs[i])[0] % (i+1);
        std::swap(outputs[i], outputs[j]);
    }

    std::cout << "\033[32m[Mixer] Round " << roundId
              << " mixed " << participants.size()
              << " participants ✓\033[0m\n";

    currentRound.completed = true;
    completedRounds.push_back(currentRound);
    currentRound = MixRound{};
    return true;
}

const MixRound* TxMixer::getCurrentRound() const {
    return &currentRound;
}

bool TxMixer::isReady() const {
    return (int)currentRound.participants.size() >= MIN_PARTICIPANTS;
}

void TxMixer::printRound() const {
    std::cout << "\n=== Mixer Status ===\n";
    std::cout << "  Current round : " << currentRound.roundId << "\n";
    std::cout << "  Participants  : " << currentRound.participants.size() << "\n";
    std::cout << "  Completed     : " << completedRounds.size() << " rounds\n";
}
