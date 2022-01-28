# Builds:
#  - Two contract wasms (release and debug)
#  - One test wasm
#  - One abi

macro(add_contract contractName)
    # Include directories can be the same in each contract by default
    set(INCLUDE_DIRS "include/" ${CMAKE_CURRENT_LIST_DIR} "../")


    # Make library for this contract
    add_library(${contractName}-lib ${CONTRACT_FILES})
    target_include_directories(${contractName}-lib PUBLIC ${INCLUDE_DIRS})
    target_link_libraries(${contractName}-lib PUBLIC eosio-contract-simple-malloc)


    # Make debug library for this contract
    add_library(${contractName}-lib-debug ${CONTRACT_FILES})
    target_include_directories(${contractName}-lib-debug PUBLIC ${INCLUDE_DIRS})
    target_link_libraries(${contractName}-lib-debug PUBLIC eosio-contract-simple-malloc-debug)


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
    add_executable(${contractName} src/_dispatcher.cpp)
    target_link_libraries(${contractName} ${contractName}-lib eosio-contract-simple-malloc)


    # Builds contract-debug.wasm
    #
    # This is like contract.wasm, but includes debugging information.
    # Debug contracts can't normally be installed on chains using `set code`.
    # Instead, cltester loads them using its `-s/--subst` option.
    #
    # Create a debugging contract by linking to either
    # eosio-contract-simple-malloc-debug or eosio-contract-full-malloc-debug.
    add_executable(${contractName}-debug src/_dispatcher.cpp)
    target_link_libraries(${contractName}-debug ${contractName}-lib-debug eosio-contract-simple-malloc-debug)


    # Generate ${contractName}.abi
    # This is a 2-step process:
    #   * Build ${contractName}.abi.wasm. This must link to eosio-contract-abigen.
    #   * Run the wasm to generate the abi
    add_executable(${contractName}-abigen src/_dispatcher.cpp)
    target_include_directories(${contractName}-abigen PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${contractName}-abigen eosio-contract-abigen)

    add_custom_command(TARGET ${contractName}-abigen POST_BUILD
        COMMAND cltester ../../${ARTIFACTS_DIR}/${contractName}-abigen.wasm > ../../${ARTIFACTS_DIR}/${contractName}.abi
    )


    # Builds test-${contractName}.wasm
    # Tests must link to either cltestlib (runs faster) or cltestlib-debug
    # (shows stack traces on failure).
    set(TEST_PROJ test-${contractName})
    add_executable(${TEST_PROJ} ${TEST_FILES})
    target_include_directories(${TEST_PROJ} PRIVATE "test/include/" ${INCLUDE_DIRS} )
    target_link_libraries(${TEST_PROJ} cltestlib-debug)

    # ctest rule which runs test-${contractName}.wasm. The -v and -s
    # options provide detailed logging. ctest hides this detail;
    # use `ctest -V` so show it.
    enable_testing()
    add_test(
        NAME ${contractName}_TEST
        COMMAND cltester ${ARTIFACTS_DIR}/${TEST_PROJ}.wasm -s
    )


endmacro()