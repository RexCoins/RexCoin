#pragma once
#include <string>
#include <vector>

// RexScript → RexVM Bytecode Compiler
// Simple high-level language that compiles to VM opcodes

struct CompileError {
    int         line;
    std::string message;
};

class Compiler {
public:
    // Compile RexScript source code to bytecode
    std::string compile(const std::string& source);

    // Check if last compile had errors
    bool hasErrors() const { return !errors.empty(); }

    // Get compile errors
    const std::vector<CompileError>& getErrors() const { return errors; }

    // Disassemble bytecode back to human-readable form
    static std::string disassemble(const std::string& bytecode);

private:
    std::vector<CompileError> errors;

    std::vector<std::string> tokenize(const std::string& source);
    std::string emit(const std::string& tokens);
};
