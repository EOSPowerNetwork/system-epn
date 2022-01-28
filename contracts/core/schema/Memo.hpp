#pragma once
#include <eosio/check.hpp>
#include <eosio/eosio.hpp>
#include <string>
#include <string_view>

#include "core/errormessages.hpp"
#include "core/fixedprops.hpp"

namespace system_epn
{
    using eosio::check;
    using std::string;
    using std::string_view;

    struct Memo
    {
        Memo() = default;
        Memo(string memo)
        {
            check(validate(memo), error::memoTooLong.data());
            value = memo;
        }

        constexpr static bool validate(const std::string_view& test)
        {
            return (test.size() <= fixedProps::memo::memoSize);
        }

        string value;
    };
    EOSIO_REFLECT(Memo, value);
    EOSIO_COMPARE(Memo);

}  // namespace system_epn