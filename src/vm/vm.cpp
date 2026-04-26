#include "vm.h"
#include "opcode.h"
#include "gas.h"
#include "../crypto/sha256.h"
#include <iostream>
#include <sstream>

VM::VM() {}

std::string VM::generateAddress(const std::string& owner,
                                 const std::string& bytecode) {
    return "RXC_CONTRACT_" +
           SHA256::hash(owner + bytecode).substr(0, 20);
}

std::string VM::deployContract(const std::string& bytecode,
                                const std::string& owner,
                                uint64_t gasLimit) {
    Contract c;
    c.address  = generateAddress(owner, bytecode);
    c.bytecode = bytecode;
    c.owner    = owner;
    c.balance  = 0;
    contracts[c.address] = c;

    std::cout << "\033[32m[VM] Contract deployed: "
              << c.address << "\033[0m\n";
    (void)gasLimit;
    return c.address;
}

VMState VM::execute(const std::string& bytecode,
                    const std::string& input,
                    uint64_t gasLimit) {
    VMState state;
    state.gasLimit = gasLimit;
    GasMeter gas(gasLimit);

    // Push input onto stack
    if (!input.empty()) state.stack.push(input);

    // Simple bytecode interpreter
    // Each character is treated as an opcode for this scaffold
    for (char c : bytecode) {
        if (state.halted || state.reverted) break;

        Opcode op = (Opcode)(uint8_t)c;
        uint64_t cost = opcodeGasCost(op);

        if (!gas.consume(cost)) {
            std::cout << "\033[31m[VM] Out of gas!\033[0m\n";
            state.reverted = true;
            break;
        }

        switch (op) {
            case Opcode::HALT:
                state.halted = true;
                break;
            case Opcode::REVERT:
                state.reverted = true;
                break;
            case Opcode::SHA256:
                if (!state.stack.empty()) {
                    std::string top = state.stack.top();
                    state.stack.pop();
                    state.stack.push(SHA256::hash(top));
                }
                break;
            case Opcode::SSTORE:
                if (state.stack.size() >= 2) {
                    std::string val = state.stack.top(); state.stack.pop();
                    std::string key = state.stack.top(); state.stack.pop();
                    state.storage[key] = val;
                }
                break;
            case Opcode::SLOAD:
                if (!state.stack.empty()) {
                    std::string key = state.stack.top(); state.stack.pop();
                    auto it = state.storage.find(key);
                    state.stack.push(it != state.storage.end() ? it->second : "");
                }
                break;
            default:
                break;
        }
    }

    state.gasUsed = gas.getUsed();
    return state;
}

VMState VM::call(const std::string& address,
                 const std::string& input,
                 uint64_t gasLimit) {
    auto it = contracts.find(address);
    if (it == contracts.end()) {
        VMState err; err.reverted = true; return err;
    }
    return execute(it->second.bytecode, input, gasLimit);
}

const Contract* VM::getContract(const std::string& address) const {
    auto it = contracts.find(address);
    return (it != contracts.end()) ? &it->second : nullptr;
}

void VM::printContracts() const {
    std::cout << "\n=== Deployed Contracts (" << contracts.size() << ") ===\n";
    for (const auto& [addr, c] : contracts)
        std::cout << "  " << addr << " owner=" << c.owner.substr(0,16) << "\n";
}
