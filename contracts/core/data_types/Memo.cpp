#include "Memo.hpp"

#include <eosio/check.hpp>

#include "errormessages.hpp"
#include "fixedprops.hpp"

namespace system_epn
{
    using eosio::check;
    using std::string;
    using std::string_view;
    
    Memo::Memo(string memo)
    {
        check(validate(memo), error::memoTooLong.data());
        value = memo;
    }

    bool Memo::validate(const std::string_view& test)
    {
        return (test.size() <= fixedProps::memo::memoSize);
    }
}  // namespace system_epn