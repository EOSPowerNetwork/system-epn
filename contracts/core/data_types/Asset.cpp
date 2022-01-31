#include "Asset.hpp"

#include <algorithm>

#include <eosio/print.hpp>
#include <eosio/check.hpp>

#include "errormessages.hpp"
#include "fixedprops.hpp"

namespace system_epn {

    using eosio::asset;
    using eosio::check;
    using eosio::name;
    using eosio::print;
    using fixedProps::Assets::supportedTokens;
    using std::find_if;

    Asset::Asset(const asset& quantity) {
        check(validate(quantity), error::invalidAssetData.data());
        value = quantity;
    }

    bool Asset::validate(const asset& quantity) {
        auto IsValid = [&](const fixedProps::Assets::AssetProps& ap) -> bool {
            bool symbolMatch = (ap.sym == quantity.symbol);
            bool quantityWithinRange = (ap.maximum >= quantity.amount) && (ap.minimum <= quantity.amount);
            return symbolMatch && quantityWithinRange;
        };
        return find_if(supportedTokens.cbegin(), supportedTokens.cend(), IsValid) != supportedTokens.cend();
    }

    name Asset::getTokenContract(const Asset& a) {
        auto SymbolMatch = [&](const auto& supported) { return (supported.sym == a.value.symbol); };
        auto itr = find_if(supportedTokens.cbegin(), supportedTokens.cend(), SymbolMatch);
        check(itr != supportedTokens.cend(), "Symbol is not supported. This should never happen.");

        return itr->contract;
    }

}