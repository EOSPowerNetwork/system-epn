# Builds contract.wasm
# Contracts may link to either:
#   * eosio-contract-simple-malloc: This library builds contracts with
#     small and fast memory allocation. free() is a no-op. Most contracts
#     should use this option.
#   * eosio-contract-full-malloc: This library builds contracts with
#     full memory allocation and reuse. Using this in your contract may
#     help expose memory bugs that eosio-contract-simple-malloc hides.
#     The downsides of eosio-contract-full-malloc are that contracts
#     will be larger and slower.

add_executable(system 
src/paycontracts.cpp 
src/donations.cpp 
src/_dispatcher.cpp 
)

target_include_directories(system 
PRIVATE 
"include/" 
"schema/"
"ricardian/")

target_link_libraries(system eosio-contract-simple-malloc)


# Builds contract-debug.wasm
#
# This is like contract.wasm, but includes debugging information.
# Debug contracts can't normally be installed on chains using `set code`.
# Instead, cltester loads them using its `-s/--subst` option.
#
# Create a debugging contract by linking to either
# eosio-contract-simple-malloc-debug or eosio-contract-full-malloc-debug.
add_executable(system-debug
src/paycontracts.cpp 
src/donations.cpp 
src/_dispatcher.cpp )

target_include_directories(system-debug
PRIVATE 
"include/" 
"schema/"
"ricardian/")

target_link_libraries(system-debug eosio-contract-simple-malloc-debug)


# Generate system.abi
# This is a 2-step process:
#   * Build system.abi.wasm. This must link to eosio-contract-abigen.
#   * Run the wasm to generate the abi

add_executable(system-abigen
src/_dispatcher.cpp )

target_include_directories(system-abigen
PRIVATE
"include/" 
"schema/"
"ricardian/")

target_link_libraries(system-abigen eosio-contract-abigen)
add_custom_command(TARGET system-abigen POST_BUILD
    COMMAND cltester ../../${ARTIFACTS_DIR}/system-abigen.wasm > ../../${ARTIFACTS_DIR}/system.abi
 )
