#pragma once
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <cstdint>

// RexVM — Stack-based Smart Contract Virtual Machine
// Similar to Bitcoin Script but Turing-complete (like EVM)

struct VMState {
    std::stack<std::string>              stack;
    std::unordered_map<std::string,std::string> storage; // persistent contract storage
    uint64_t gasUsed   = 0;
    uint64_t gasLimit  = 0;
    bool     halted    = false;
    bool     reverted  = false;
    std::string returnData;
};

struct Contract {
    std::string address;
    std::string bytecode;   // Compiled contract
    std::string owner;
    uint64_t    balance;
    std::unordered_map<std::string,std::string> storage;
};

class VM {
public:
    VM();

    // Deploy a new contract
    std::string deployContract(const std::string& bytecode,
                                const std::string& owner,
                                uint64_t gasLimit);

    // Call a deployed contract
    VMState call(const std::string& contractAddress,
                 const std::string& input,
                 uint64_t gasLimit);

    // Get contract by address
    const Contract* getContract(const std::string& address) const;

    // Execute bytecode in a VM state
    VMState execute(const std::string& bytecode,
                    const std::string& input,
                    uint64_t gasLimit);

    void printContracts() const;

private:
    std::unordered_map<std::string, Contract> contracts;
    std::string generateAddress(const std::string& owner,
                                const std::string& bytecode);
};
