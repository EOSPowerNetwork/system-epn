# Regular version of core library
add_library(core
"data_types/Asset.cpp"
"data_types/Frequency.cpp"
"data_types/Memo.cpp"
)
target_include_directories(core PUBLIC # Todo <-- Make these private ---------------------------------
${CMAKE_CURRENT_LIST_DIR}
"data_types/include/"
)
target_link_libraries(core PUBLIC eosio-core)


# Debug version
add_library(core-debug
"data_types/Asset.cpp"
"data_types/Frequency.cpp"
"data_types/Memo.cpp"
)

target_include_directories(core-debug PUBLIC
${CMAKE_CURRENT_LIST_DIR}
"data_types/include/"
)
target_link_libraries(core-debug PUBLIC eosio-core-debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)