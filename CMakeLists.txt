cmake_minimum_required(VERSION 3.16)
project(RexCoin VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    src/core/block.cpp
    src/core/blockchain.cpp
    src/core/transaction.cpp
    src/core/utxo.cpp
    src/core/merkle.cpp
)

set(CRYPTO_SOURCES
    src/crypto/sha256.cpp
    src/crypto/sha3.cpp
    src/crypto/ecdsa.cpp
    src/crypto/bls.cpp
    src/crypto/wallet.cpp
)

set(CONSENSUS_SOURCES
    src/consensus/pow.cpp
    src/consensus/pos.cpp
    src/consensus/hybrid.cpp
    src/consensus/difficulty.cpp
)

set(NETWORK_SOURCES
    src/network/node.cpp
    src/network/peer.cpp
    src/network/protocol.cpp
    src/network/sync.cpp
    src/network/tor.cpp
)

set(VM_SOURCES
    src/vm/vm.cpp
    src/vm/opcode.cpp
    src/vm/gas.cpp
    src/vm/compiler.cpp
)

set(PRIVACY_SOURCES
    src/privacy/zkproof.cpp
    src/privacy/stealth.cpp
    src/privacy/mixer.cpp
)

set(LAYER2_SOURCES
    src/layer2/channel.cpp
    src/layer2/lightning.cpp
    src/layer2/sidechain.cpp
)

set(GOVERNANCE_SOURCES
    src/governance/proposal.cpp
    src/governance/vote.cpp
    src/governance/dao.cpp
)

set(STORAGE_SOURCES
    src/storage/leveldb.cpp
    src/storage/cache.cpp
    src/storage/pruning.cpp
)

set(API_SOURCES
    src/api/rpc.cpp
    src/api/rest.cpp
    src/api/websocket.cpp
)

set(ALL_SOURCES
    ${CORE_SOURCES}
    ${CRYPTO_SOURCES}
    ${CONSENSUS_SOURCES}
    ${NETWORK_SOURCES}
    ${VM_SOURCES}
    ${PRIVACY_SOURCES}
    ${LAYER2_SOURCES}
    ${GOVERNANCE_SOURCES}
    ${STORAGE_SOURCES}
    ${API_SOURCES}
)

# Main executable
add_executable(rexcoin src/main.cpp ${ALL_SOURCES})
target_include_directories(rexcoin PRIVATE src)
target_compile_options(rexcoin PRIVATE -Wall -Wextra -O2 -Wno-unused-private-field -Wno-unused-but-set-variable)

# Tests
enable_testing()

add_executable(core_test tests/core_test.cpp ${CORE_SOURCES} ${CRYPTO_SOURCES} ${CONSENSUS_SOURCES})
target_include_directories(core_test PRIVATE src)
add_test(NAME core_test COMMAND core_test)

add_executable(crypto_test tests/crypto_test.cpp ${CORE_SOURCES} ${CRYPTO_SOURCES} ${CONSENSUS_SOURCES})
target_include_directories(crypto_test PRIVATE src)
add_test(NAME crypto_test COMMAND crypto_test)

add_executable(network_test tests/network_test.cpp ${CORE_SOURCES} ${CRYPTO_SOURCES} ${CONSENSUS_SOURCES} ${NETWORK_SOURCES})
target_include_directories(network_test PRIVATE src)
add_test(NAME network_test COMMAND network_test)

add_executable(vm_test tests/vm_test.cpp ${CORE_SOURCES} ${CRYPTO_SOURCES} ${CONSENSUS_SOURCES} ${VM_SOURCES})
target_include_directories(vm_test PRIVATE src)
add_test(NAME vm_test COMMAND vm_test)
