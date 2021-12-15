#pragma once

#include <algorithm>
#include <eosio/asset.hpp>
#include <eosio/check.hpp>
#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

#include "errormessages.hpp"
#include "fixedprops.hpp"

namespace system_epn {
    using eosio::asset;
    using eosio::check;
    using eosio::name;
    using eosio::print;
    using fixedProps::Assets::supportedTokens;
    using std::find_if;

    struct Asset {
        Asset() = default;
        Asset(asset quantity)
        {
            check(validate(quantity), error::invalidAssetData.data());
            value = quantity;
        }

        constexpr static bool validate(const asset& quantity)
        {
            auto IsValid = [&](const fixedProps::Assets::AssetProps& ap) -> bool {
                bool symbolMatch = (ap.sym == quantity.symbol);
                bool quantityWithinRange = (ap.maximum >= quantity.amount) && (ap.minimum <= quantity.amount);
                return symbolMatch && quantityWithinRange;
            };
            return find_if(supportedTokens.cbegin(), supportedTokens.cend(), IsValid) != supportedTokens.cend();
        }

        static name getTokenContract(const Asset& a)
        {
            auto SymbolMatch = [&](const auto& supported) { return (supported.sym == a.value.symbol); };
            auto itr = find_if(supportedTokens.cbegin(), supportedTokens.cend(), SymbolMatch);
            check(itr != supportedTokens.cend(), "Symbol is not supported. This should never happen.");

            return itr->contract;
        }

        eosio::asset value;
    };
    EOSIO_REFLECT(Asset, value);
    EOSIO_COMPARE(Asset);

}  // namespace system_epn