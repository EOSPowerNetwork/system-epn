#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>
#include <utility>

#include "Donations.hpp"

namespace system_epn {
    using namespace eosio;
    using std::pair;

    extern const char* draftdon_ricardian;
    extern const char* signdon_ricardian;
    extern const char* ricardian_clause2;

    pair<Asset, Asset> _splitPayment(const Asset& payment);
    void transfer(const name& from, const name& to, const permission_level& permission, const Asset& payment, const Memo& memo);

    class donations : public contract {
       public:
        using contract::contract;

        void draftdon(const name& owner, const name& contractID, const Memo& memoSuffix);
        void signdon(const name& signer, const name& drafter, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo);

        // void deletedon(const name& contractID);
        // void unsigndon(const name& contractName, const name& drafterName);
    };

}  // namespace system_epn
