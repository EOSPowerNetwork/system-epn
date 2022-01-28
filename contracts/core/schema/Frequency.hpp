#pragma once
#include <eosio/check.hpp>
#include <eosio/eosio.hpp>

#include "core/errormessages.hpp"
#include "core/fixedprops.hpp"

namespace system_epn
{
    using eosio::check;

    struct Frequency
    {
        Frequency() = default;
        Frequency(uint32_t frequency)
        {
            check(validate(frequency), error::frequencyOutOfRange.data());
            value = frequency;
        }

        constexpr static bool validate(const uint32_t& test)
        {
            bool lowerBound = (fixedProps::Frequency::minimum_frequency_seconds <= test);
            bool upperBound = (test <= fixedProps::Frequency::maximum_frequency_seconds);
            return lowerBound && upperBound;
        }

        uint32_t value;
    };
    EOSIO_REFLECT(Frequency, value);
    EOSIO_COMPARE(Frequency);

}  // namespace system_epn