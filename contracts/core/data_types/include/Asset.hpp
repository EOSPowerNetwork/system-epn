#pragma once

#include <eosio/asset.hpp>
#include <eosio/operators.hpp>
#include <eosio/reflection.hpp>

namespace system_epn
{
    struct Asset {
        Asset() = default;
        Asset(const eosio::asset& quantity);

        static bool validate(const eosio::asset& quantity);
        static eosio::name getTokenContract(const Asset& a);

        eosio::asset value;
    };
    EOSIO_REFLECT(Asset, value);
    EOSIO_COMPARE(Asset);

}  // namespace system_epn