#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>

#include <string>
#include <vector>

namespace system_epn
{
    using eosio::asset;
    using eosio::name;
    using std::string;
    using std::vector;

    struct SignerData
    {
        asset quantity;
        uint32_t frequency;
        string signerMemo;
    };
    EOSIO_REFLECT(SignerData, quantity, frequency, signerMemo);

    // This table is scoped to the owner, so owner does not need to be part of the table
    struct Donations
    {
        name contractID;
        string memoSuffix;
        bool drafterPaysSignerRAM;
        std::vector<SignerData> signerData;

        uint64_t primary_key() const { return contractID.value; }
    };
    EOSIO_REFLECT(Donations, contractID, memoSuffix, drafterPaysSignerRAM, signerData);
}  // namespace system_epn