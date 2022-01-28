cmake_minimum_required(VERSION 3.16)

set(PROJ epn)
project(${PROJ})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Libraries for building contracts and tests
find_package(clsdk REQUIRED)

# Set output directory
set(ARTIFACTS_DIR "artifacts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${ARTIFACTS_DIR})

# Build projects as well
add_subdirectory(contracts/system)

# Builds test-${PROJ}.wasm
# Tests must link to either cltestlib (runs faster) or cltestlib-debug
# (shows stack traces on failure).
set(TEST_PROJ test-${PROJ})

add_executable(${TEST_PROJ}
test/${TEST_PROJ}.cpp
test/helpers.cpp
test/epn_test_chain.cpp)

target_include_directories(${TEST_PROJ}
PRIVATE
"contracts/system/include/" 
"contracts/system/schema/"
"test/include"
)
target_link_libraries(${TEST_PROJ} cltestlib-debug)

# ctest rule which runs test-${PROJ}.wasm. The -v and -s
# options provide detailed logging. ctest hides this detail;
# use `ctest -V` so show it.
enable_testing()
add_test(
    NAME ${PROJ}_TEST
    COMMAND cltester ${ARTIFACTS_DIR}/${TEST_PROJ}.wasm -s
)

# These symlinks help keep absolute paths outside of the files in .vscode/
execute_process(COMMAND ln -sf ${clsdk_DIR} ${CMAKE_CURRENT_BINARY_DIR}/clsdk)
execute_process(COMMAND ln -sf ${WASI_SDK_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/wasi-sdk)

# Generate compile_commands.json during build to enable
# symbol lookup in some editors
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
