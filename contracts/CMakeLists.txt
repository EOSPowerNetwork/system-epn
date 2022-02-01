set(CONTRACT_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# Import the function that adds a contract
list(APPEND  CMAKE_MODULE_PATH ${${PROJ}_SOURCE_DIR} )
include(add_contract)

# Adds contracts (and core lib)
add_subdirectory(core)
add_subdirectory(system)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)