#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

// On-chain Governance — RXC holders vote on protocol changes

enum class ProposalStatus {
    ACTIVE,
    PASSED,
    REJECTED,
    EXPIRED
};

struct Proposal {
    std::string    id;
    std::string    title;
    std::string    description;
    std::string    proposer;
    uint64_t       votesFor;
    uint64_t       votesAgainst;
    ProposalStatus status;
    int64_t        expiresAt;    // Unix timestamp
    std::string    changeParam;  // What parameter to change
    std::string    changeValue;  // New value
};

class GovernanceSystem {
public:
    static constexpr double PASS_THRESHOLD    = 0.67; // 67% to pass
    static constexpr uint64_t MIN_STAKE_VOTE  = 100ULL * 100000000ULL; // 100 RXC

    // Submit a new proposal
    std::string submitProposal(const std::string& proposer,
                                const std::string& title,
                                const std::string& description,
                                const std::string& changeParam,
                                const std::string& changeValue,
                                int64_t durationSeconds = 604800); // 1 week

    // Vote on a proposal
    bool vote(const std::string& proposalId,
              const std::string& voter,
              uint64_t voterStake,
              bool inFavor);

    // Tally votes and update status
    void tally(const std::string& proposalId);

    // Get proposal
    const Proposal* getProposal(const std::string& id) const;

    // Get all active proposals
    std::vector<Proposal> getActiveProposals() const;

    void print() const;

private:
    std::unordered_map<std::string, Proposal> proposals;
    std::unordered_map<std::string, std::string> voted; // voter+proposalId→vote

    std::string generateProposalId(const std::string& proposer,
                                    const std::string& title);
};
