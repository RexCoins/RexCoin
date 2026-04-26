#include "compiler.h"
#include "opcode.h"
#include <sstream>
#include <iostream>

std::vector<std::string> Compiler::tokenize(const std::string& source) {
    std::vector<std::string> tokens;
    std::istringstream iss(source);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

std::string Compiler::compile(const std::string& source) {
    errors.clear();
    auto tokens = tokenize(source);
    std::string bytecode;
    int line = 1;

    for (const auto& token : tokens) {
        if      (token == "PUSH")     bytecode += (char)(uint8_t)Opcode::PUSH;
        else if (token == "POP")      bytecode += (char)(uint8_t)Opcode::POP;
        else if (token == "ADD")      bytecode += (char)(uint8_t)Opcode::ADD;
        else if (token == "SUB")      bytecode += (char)(uint8_t)Opcode::SUB;
        else if (token == "MUL")      bytecode += (char)(uint8_t)Opcode::MUL;
        else if (token == "DIV")      bytecode += (char)(uint8_t)Opcode::DIV;
        else if (token == "SHA256")   bytecode += (char)(uint8_t)Opcode::SHA256;
        else if (token == "SSTORE")   bytecode += (char)(uint8_t)Opcode::SSTORE;
        else if (token == "SLOAD")    bytecode += (char)(uint8_t)Opcode::SLOAD;
        else if (token == "TRANSFER") bytecode += (char)(uint8_t)Opcode::TRANSFER;
        else if (token == "HALT")     bytecode += (char)(uint8_t)Opcode::HALT;
        else if (token == "REVERT")   bytecode += (char)(uint8_t)Opcode::REVERT;
        else if (token == "RETURN")   bytecode += (char)(uint8_t)Opcode::RETURN;
        else if (token == "\n")       line++;
        else {
            // Treat as data push — embed as literal string
            bytecode += (char)(uint8_t)Opcode::PUSH;
            bytecode += token;
        }
    }
    return bytecode;
}

std::string Compiler::disassemble(const std::string& bytecode) {
    std::stringstream ss;
    for (size_t i = 0; i < bytecode.size(); i++) {
        Opcode op = (Opcode)(uint8_t)bytecode[i];
        ss << "[" << i << "] " << opcodeName(op)
           << " (gas=" << opcodeGasCost(op) << ")\n";
    }
    return ss.str();
}
