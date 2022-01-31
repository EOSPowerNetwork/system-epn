#pragma once

#include <eosio/operators.hpp>
#include <eosio/reflection.hpp>

namespace system_epn
{
    struct Frequency
    {
        Frequency() = default;
        Frequency(uint32_t frequency);

        static bool validate(const uint32_t& test);

        uint32_t value;
    };
    EOSIO_REFLECT(Frequency, value);
    EOSIO_COMPARE(Frequency);

}  // namespace system_epn