#include "Frequency.hpp"

#include <eosio/check.hpp>
#include "errormessages.hpp"
#include "fixedprops.hpp"

namespace system_epn
{
    using eosio::check;


    Frequency::Frequency(uint32_t frequency)
    {
        check(validate(frequency), error::frequencyOutOfRange.data());
        value = frequency;
    }

    bool Frequency::validate(const uint32_t& test)
    {
        bool lowerBound = (fixedProps::Frequency::minimum_frequency_seconds <= test);
        bool upperBound = (test <= fixedProps::Frequency::maximum_frequency_seconds);
        return lowerBound && upperBound;
    }

}  // namespace system_epn