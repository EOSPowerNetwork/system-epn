#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>

#include "Donations.hpp"

namespace system_epn
{
    using std::string;
    using namespace eosio;
    using system_epn::Donations;

    extern const char* draftdon_ricardian;
    extern const char* signdon_ricardian;
    extern const char* ricardian_clause2;

    class donations : public contract
    {
       public:
        using contract::contract;

        donations(name receiver, name code, datastream<const char*> ds);

        /*
        */
        void draftdon(const name& contractID);
        // void deletedon(const name& contractID);

        // void signdon(const name& to, const asset& amount, const uint32_t& frequency);
        // void unsigndon(const name& contractName, const name& drafterName);

        using DonationsTable = eosio::multi_index<"donations"_n, Donations>;
    };
}  // namespace system_epn
