#include "proposal.h"
#include "../crypto/sha256.h"
#include <iostream>
#include <chrono>

std::string GovernanceSystem::generateProposalId(const std::string& proposer,
                                                   const std::string& title) {
    return SHA256::hash(proposer + title + std::to_string(proposals.size())).substr(0,12);
}

std::string GovernanceSystem::submitProposal(
    const std::string& proposer, const std::string& title,
    const std::string& description, const std::string& changeParam,
    const std::string& changeValue, int64_t durationSeconds)
{
    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    Proposal p;
    p.id           = generateProposalId(proposer, title);
    p.title        = title;
    p.description  = description;
    p.proposer     = proposer;
    p.votesFor     = 0;
    p.votesAgainst = 0;
    p.status       = ProposalStatus::ACTIVE;
    p.expiresAt    = now + durationSeconds;
    p.changeParam  = changeParam;
    p.changeValue  = changeValue;
    proposals[p.id] = p;

    std::cout << "\033[32m[Gov] Proposal submitted: " << p.id
              << " \"" << title << "\"\033[0m\n";
    return p.id;
}

bool GovernanceSystem::vote(const std::string& proposalId,
                             const std::string& voter,
                             uint64_t voterStake,
                             bool inFavor) {
    auto it = proposals.find(proposalId);
    if (it == proposals.end()) return false;
    if (it->second.status != ProposalStatus::ACTIVE) return false;
    if (voterStake < MIN_STAKE_VOTE) {
        std::cout << "\033[31m[Gov] Stake too low to vote\033[0m\n";
        return false;
    }

    std::string voteKey = voter + proposalId;
    if (voted.count(voteKey)) {
        std::cout << "\033[31m[Gov] Already voted\033[0m\n";
        return false;
    }
    voted[voteKey] = inFavor ? "yes" : "no";

    if (inFavor) it->second.votesFor     += voterStake;
    else         it->second.votesAgainst += voterStake;

    std::cout << "\033[36m[Gov] Vote recorded: "
              << (inFavor ? "FOR" : "AGAINST") << " ("
              << voterStake/1e8 << " RXC weight)\033[0m\n";
    tally(proposalId);
    return true;
}

void GovernanceSystem::tally(const std::string& proposalId) {
    auto it = proposals.find(proposalId);
    if (it == proposals.end()) return;
    Proposal& p = it->second;
    uint64_t total = p.votesFor + p.votesAgainst;
    if (total == 0) return;
    double ratio = (double)p.votesFor / total;
    if (ratio >= PASS_THRESHOLD) {
        p.status = ProposalStatus::PASSED;
        std::cout << "\033[32m[Gov] Proposal PASSED! Applying: "
                  << p.changeParam << "=" << p.changeValue << "\033[0m\n";
    }
}

const Proposal* GovernanceSystem::getProposal(const std::string& id) const {
    auto it = proposals.find(id);
    return (it != proposals.end()) ? &it->second : nullptr;
}

std::vector<Proposal> GovernanceSystem::getActiveProposals() const {
    std::vector<Proposal> result;
    for (const auto& [id, p] : proposals)
        if (p.status == ProposalStatus::ACTIVE)
            result.push_back(p);
    return result;
}

void GovernanceSystem::print() const {
    std::cout << "\n=== Governance ===\n";
    for (const auto& [id, p] : proposals) {
        std::cout << "  [" << id << "] " << p.title;
        switch(p.status) {
            case ProposalStatus::ACTIVE:   std::cout << " [ACTIVE]\n";   break;
            case ProposalStatus::PASSED:   std::cout << " [PASSED ✓]\n"; break;
            case ProposalStatus::REJECTED: std::cout << " [REJECTED]\n"; break;
            default:                       std::cout << " [EXPIRED]\n";  break;
        }
        std::cout << "     For=" << p.votesFor/1e8
                  << " Against=" << p.votesAgainst/1e8 << " RXC\n";
    }
}
