#pragma once

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>

#include <string>
#include <vector>

namespace system_epn
{
    using eosio::name;
    using std::string;
    using std::vector;

    // This table is scoped to the owner, so owner does not need to be part of the table
    struct Donations
    {
        name contractID;
        string memoSuffix;

        // Todo - where to store those who sign this contract?

        uint64_t primary_key() const { return contractID.value; }
    };
    EOSIO_REFLECT(Donations, contractID, memoSuffix);
}  // namespace system_epn