#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>
#include <string>
#include <vector>

namespace system_epn {
    using eosio::check;
    using eosio::contract;
    using eosio::datastream;
    using eosio::name;
    using eosio::print;
    using eosio::symbol;
    using std::string;

    // Ricardian contracts live in ricardian/paycontracts-ricardian.cpp
    extern const char* addasset_ricardian;
    extern const char* ricardian_clause;

    class paycontracts : public contract {
       public:
        using eosio::contract::contract;

        paycontracts(name receiver, name code, datastream<const char*> ds);

        void addasset(const symbol& type, const name& contract, const int64_t& minAmount, const int64_t& maxAmount);
    };
}  // namespace system_epn