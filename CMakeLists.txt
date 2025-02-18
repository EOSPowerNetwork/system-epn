cmake_minimum_required(VERSION 3.16)

set(PROJ epn)
project(${PROJ})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Libraries for building contracts and tests
find_package(clsdk REQUIRED)

# Todo - organize artifact output
# Set output directory
set(ARTIFACTS_DIR "artifacts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${ARTIFACTS_DIR})

add_subdirectory(contracts)

# These symlinks help keep absolute paths outside of the files in .vscode/
execute_process(COMMAND ln -sf ${clsdk_DIR} ${CMAKE_CURRENT_BINARY_DIR}/clsdk)
execute_process(COMMAND ln -sf ${WASI_SDK_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/wasi-sdk)

# Generate compile_commands.json during build to enable
# symbol lookup in some editors
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
