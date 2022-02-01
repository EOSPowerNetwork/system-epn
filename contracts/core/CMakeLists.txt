
function(add_core_lib suffix)
    add_library(core${suffix}
        "data_types/Asset.cpp"
        "data_types/Frequency.cpp"
        "data_types/Memo.cpp"
    )
    target_include_directories(core${suffix} 
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        "data_types/include/"
    )
    target_link_libraries(core${suffix}
        PUBLIC
        eosio-core${suffix}
    )
endfunction()

add_core_lib("")
add_core_lib("-debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS on)