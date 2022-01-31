# Builds:
#  - Two contract wasms (release and debug)
#  - One test wasm
#  - One abi

macro(add_contract contractName)

    # Include directories can be the same in each contract by default
    set(INCLUDE_DIRS "include/" ${CMAKE_CURRENT_LIST_DIR} "../")

    # Make library for RAM interface classes
    # Release
    add_library(${contractName}-interface-lib ${INTERFACE_FILES})
    target_link_libraries(${contractName}-interface-lib PUBLIC core)
    target_include_directories(${contractName}-interface-lib PRIVATE "interface/include/" ${CONTRACT_ROOT} ${clsdk_DIR}/eosiolib/contracts/include)
    
    # Debug
    add_library(${contractName}-interface-lib-debug ${INTERFACE_FILES})
    target_link_libraries(${contractName}-interface-lib-debug PUBLIC core-debug)
    target_include_directories(${contractName}-interface-lib-debug PRIVATE "interface/include/" ${CONTRACT_ROOT} ${clsdk_DIR}/eosiolib/contracts/include)

    # Make library for this contract
    # Release
    add_library(${contractName}-lib ${CONTRACT_FILES})
    target_link_libraries(${contractName}-lib PUBLIC core ${contractName}-interface-lib)
    target_include_directories(${contractName}-lib PUBLIC ${INCLUDE_DIRS} ${clsdk_DIR}/eosiolib/contracts/include ${clsdk_DIR}/contracts/token/include)

    # Debug
    add_library(${contractName}-lib-debug ${CONTRACT_FILES})
    target_link_libraries(${contractName}-lib-debug PUBLIC core-debug ${contractName}-interface-lib-debug )
    target_include_directories(${contractName}-lib-debug PUBLIC ${INCLUDE_DIRS} ${clsdk_DIR}/eosiolib/contracts/include ${clsdk_DIR}/contracts/token/include)
    
    
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
    target_link_libraries(${contractName} ${contractName}-lib core eosio-contract-simple-malloc)


    # Builds contract-debug.wasm
    #
    # This is like contract.wasm, but includes debugging information.
    # Debug contracts can't normally be installed on chains using `set code`.
    # Instead, cltester loads them using its `-s/--subst` option.
    #
    # Create a debugging contract by linking to either
    # eosio-contract-simple-malloc-debug or eosio-contract-full-malloc-debug.
    add_executable(${contractName}-debug src/_dispatcher.cpp)
    target_link_libraries(${contractName}-debug ${contractName}-lib-debug core-debug eosio-contract-simple-malloc-debug)


    # Generate ${contractName}.abi
    # This is a 2-step process:
    #   * Build ${contractName}.abi.wasm. This must link to eosio-contract-abigen.
    #   * Run the wasm to generate the abi
    add_executable(${contractName}-abigen src/_dispatcher.cpp)
    target_link_libraries(${contractName}-abigen eosio-contract-abigen)
    target_include_directories(${contractName}-abigen PRIVATE ${INCLUDE_DIRS})

    add_custom_command(TARGET ${contractName}-abigen POST_BUILD
        COMMAND cltester ../../${ARTIFACTS_DIR}/${contractName}-abigen.wasm > ../../${ARTIFACTS_DIR}/${contractName}.abi
    )


    # Builds test-${contractName}.wasm
    # Tests must link to either cltestlib (runs faster) or cltestlib-debug
    # (shows stack traces on failure).
    add_executable(test-${contractName} ${TEST_FILES} )
    target_link_libraries(test-${contractName} core-debug cltestlib-debug ${contractName}-lib-debug)
    target_include_directories(test-${contractName} PRIVATE ${INCLUDE_DIRS} "test/include/")

    # ctest rule which runs test-${contractName}.wasm. The -v and -s
    # options provide detailed logging. ctest hides this detail;
    # use `ctest -V` so show it.
    enable_testing()
    add_test(
        NAME ${contractName}_TEST
        COMMAND cltester ${ARTIFACTS_DIR}/test-${contractName}.wasm -s
    )


    set(CMAKE_EXPORT_COMPILE_COMMANDS on)

endmacro()