#pragma once
#include <string>
// Vote record — stored on-chain
struct VoteRecord {
    std::string proposalId;
    std::string voter;
    bool        inFavor;
    uint64_t    weight;      // Stake weight at time of vote
    int64_t     timestamp;
    std::string signature;   // Proves vote authenticity
};
