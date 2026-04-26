#include <iostream>
#include <cassert>
#include "../src/vm/vm.h"
#include "../src/vm/compiler.h"
#include "../src/vm/opcode.h"

void test_compiler() {
    Compiler c;
    std::string bytecode = c.compile("PUSH SHA256 HALT");
    assert(!bytecode.empty());
    assert(!c.hasErrors());
    std::cout << "[PASS] Compiler\n";
}

void test_vm_deploy() {
    VM vm;
    Compiler c;
    std::string bc = c.compile("HALT");
    std::string addr = vm.deployContract(bc, "RXCOwner", 100000);
    assert(!addr.empty());
    assert(addr.substr(0,12) == "RXC_CONTRACT");
    std::cout << "[PASS] Contract deploy\n";
}

void test_vm_execute() {
    VM vm;
    Compiler c;
    std::string bc = c.compile("SHA256 HALT");
    std::string addr = vm.deployContract(bc, "RXCOwner", 100000);
    VMState state = vm.call(addr, "test_input", 100000);
    assert(state.halted);
    assert(!state.reverted);
    assert(state.gasUsed > 0);
    std::cout << "[PASS] VM execute\n";
}

void test_gas_limit() {
    VM vm;
    Compiler c;
    // Many SHA256 ops to exhaust gas
    std::string src;
    for(int i=0;i<100;i++) src += "SHA256 ";
    src += "HALT";
    std::string bc = c.compile(src);
    std::string addr = vm.deployContract(bc, "RXCOwner", 1000000);
    VMState state = vm.call(addr, "input", 50); // Very low gas
    assert(state.reverted); // Should run out of gas
    std::cout << "[PASS] Gas limit\n";
}

int main() {
    std::cout << "\n=== RexCoin VM Tests ===\n\n";
    test_compiler();
    test_vm_deploy();
    test_vm_execute();
    test_gas_limit();
    std::cout << "\n[ALL TESTS PASSED] ✓\n";
    return 0;
}
