#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>

#include "Donations.hpp"

namespace system_epn {
    using std::string;
    using namespace eosio;
    using system_epn::DonationsTable;

    extern const char* draftdon_ricardian;
    extern const char* signdon_ricardian;
    extern const char* ricardian_clause2;

    class donations : public contract {
       public:
        using contract::contract;

        void draftdon(const name& owner, const name& contractID, const Memo& memoSuffix);
        void signdon(const name& signer, const name& drafter, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo);

        // void deletedon(const name& contractID);
        // void unsigndon(const name& contractName, const name& drafterName);

       private:
        DonationsTable _getDonations(const name& owner);
    };

}  // namespace system_epn
