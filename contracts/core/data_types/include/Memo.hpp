#pragma once

#include <eosio/operators.hpp>
#include <eosio/reflection.hpp>

#include <string>
#include <string_view>

namespace system_epn
{
    struct Memo
    {
        Memo() = default;
        Memo(std::string memo);

        static bool validate(const std::string_view& test);

        std::string value;
    };
    EOSIO_REFLECT(Memo, value);
    EOSIO_COMPARE(Memo);

}  // namespace system_epn