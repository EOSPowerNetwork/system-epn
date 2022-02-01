# Builds:
#  - Two contract wasms (release and debug)
#  - One test wasm
#  - One abi

macro(add_contract contractName)

    # Include directories can be the same in each contract by default
    set(INCLUDE_DIRS "include/" ${CMAKE_CURRENT_LIST_DIR} "../")

    function(add_version suffix)
        # Interface libraries
        add_library(${contractName}-interface-lib${suffix}
            ${INTERFACE_FILES}
        )
        target_link_libraries(${contractName}-interface-lib${suffix}
            PUBLIC
            core${suffix}
        )
        target_include_directories(${contractName}-interface-lib${suffix}
            PRIVATE
            "interface/include/"
            ${CMAKE_CURRENT_LIST_DIR}
            ${CONTRACT_ROOT}
            ${clsdk_DIR}/eosiolib/contracts/include
        )

        # Contract libraries
        add_library(${contractName}-lib${suffix}
            ${CONTRACT_FILES}
        )
        target_link_libraries(${contractName}-lib${suffix}
            PUBLIC
            core${suffix}
            ${contractName}-interface-lib${suffix}
        )
        target_include_directories(${contractName}-lib${suffix}
            PUBLIC
            ${INCLUDE_DIRS}
            ${clsdk_DIR}/eosiolib/contracts/include
            ${clsdk_DIR}/contracts/token/include
        )

        # Contract wasm
        add_executable(${contractName}${suffix} src/_dispatcher.cpp)
        target_link_libraries(${contractName}${suffix}
            # eosio-contract-full-malloc${suffix} # help expose memory bugs at the expense of contract speed and size
            eosio-contract-simple-malloc${suffix}    
            ${contractName}-lib${suffix}
            core${suffix}
        )
    
    endfunction()

    add_version("")
    add_version("-debug")


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
    target_link_libraries(test-${contractName}
        cltestlib-debug     
        core-debug
        ${contractName}-lib-debug 
    )
    target_include_directories(test-${contractName}
        PRIVATE
        ${INCLUDE_DIRS}
        "test/include/"
    )
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