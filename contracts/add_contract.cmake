# Builds:
#  - Two contract wasms (release and debug)
#  - One test wasm
#  - One abi

macro(add_contract contractName)

    # Include directories can be the same in each contract by default
    set(INCLUDE_DIRS "contract/include/" ${CMAKE_CURRENT_LIST_DIR} "../")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${contractName})

    function(add_version suffix)

        # Separate debug artifacts into directory
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${suffix})

        # Interface libraries
        if (DEFINED INTERFACE_FILES)
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
        endif()

        # Contract libraries
        add_library(${contractName}-lib${suffix}
            ${CONTRACT_FILES}
        )
        target_link_libraries(${contractName}-lib${suffix}
            PUBLIC
            core${suffix}
        )
        if (DEFINED INTERFACE_FILES)
            target_link_libraries(${contractName}-lib${suffix}
                PUBLIC
                ${contractName}-interface-lib${suffix}
            )
        endif()
        target_include_directories(${contractName}-lib${suffix}
            PUBLIC
            ${INCLUDE_DIRS}
            ${clsdk_DIR}/eosiolib/contracts/include
            ${clsdk_DIR}/contracts/token/include
        )

        # Contract wasm <-- The dispatcher should eventually be merged with the contract files once multi-class contracts are supported.
        #                   When that happens, this target should merge with Contract libraries target
        add_executable(${contractName}${suffix} _dispatcher.cpp)
        target_link_libraries(${contractName}${suffix}
            # eosio-contract-full-malloc${suffix} # help expose memory bugs at the expense of contract speed and size
            eosio-contract-simple-malloc${suffix}
            ${contractName}-lib${suffix}
        )    
        # To preprocess, run "make _dispatcher.cpp.i -j $(nproc)" in the directory with the Makefile that understands the corresponding .i target
    
    endfunction()

    add_version("")
    add_version("-debug")

    # The rest of the artifacts in this file are debug artifacts
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}-debug)

    # Generate ${contractName}.abi
    # This is a 2-step process:
    #   * Build ${contractName}.abi.wasm. This must link to eosio-contract-abigen.
    #   * Run the wasm to generate the abi
    add_executable(${contractName}-abigen _dispatcher.cpp)
    target_link_libraries(${contractName}-abigen eosio-contract-abigen)
    target_include_directories(${contractName}-abigen PRIVATE ${INCLUDE_DIRS})

    add_custom_command(TARGET ${contractName}-abigen POST_BUILD
        COMMAND cltester ../../${ARTIFACTS_DIR}/${contractName}-debug/${contractName}-abigen.wasm > ../../${ARTIFACTS_DIR}/${contractName}/${contractName}.abi
    )


    # Builds test-${contractName}.wasm
    # Tests must link to either cltestlib (runs faster) or cltestlib-debug
    # (shows stack traces on failure).
    if (DEFINED TEST_FILES)
        add_executable(test-${contractName} ${TEST_FILES} )
        target_link_libraries(test-${contractName}
            cltestlib-debug     
            core-debug
            #${contractName}-lib-debug 
            ${contractName}-interface-lib-debug
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
    endif()

    set(CMAKE_EXPORT_COMPILE_COMMANDS on)

endmacro()