#include "opcode.h"

std::string opcodeName(Opcode op) {
    switch(op) {
        case Opcode::PUSH:   return "PUSH";
        case Opcode::POP:    return "POP";
        case Opcode::ADD:    return "ADD";
        case Opcode::SUB:    return "SUB";
        case Opcode::MUL:    return "MUL";
        case Opcode::DIV:    return "DIV";
        case Opcode::SHA256: return "SHA256";
        case Opcode::SSTORE: return "SSTORE";
        case Opcode::SLOAD:  return "SLOAD";
        case Opcode::HALT:   return "HALT";
        case Opcode::REVERT: return "REVERT";
        case Opcode::RETURN: return "RETURN";
        case Opcode::TRANSFER: return "TRANSFER";
        default:             return "UNKNOWN";
    }
}

uint64_t opcodeGasCost(Opcode op) {
    switch(op) {
        case Opcode::PUSH: case Opcode::POP: case Opcode::DUP: return 1;
        case Opcode::ADD:  case Opcode::SUB: case Opcode::MUL: return 3;
        case Opcode::DIV:  case Opcode::MOD:                   return 5;
        case Opcode::SHA256: case Opcode::HASH:                return 30;
        case Opcode::SSTORE:                                   return 200;
        case Opcode::SLOAD:                                    return 50;
        case Opcode::TRANSFER:                                 return 100;
        case Opcode::LOG:                                      return 75;
        default:                                               return 1;
    }
}
