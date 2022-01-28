#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>
#include <utility>

#include "core/fixedprops.hpp"

#include "schema/Donations.hpp"


namespace system_epn
{
    using namespace eosio;

    // Todo - Should eventually be moved into common utility class, since it will be needed for all contract types
    void transfer(const name& from, const name& to, const permission_level& permission, const Asset& payment, const Memo& signerMemo, const Memo& drafterMemo);

    class donations : public contract {
       public:
        using contract::contract;

        void draftdon(const name& owner, const name& contractID, const Memo& memoSuffix);
        void signdon(const name& signer, const name& drafter, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo);

        // void deletedon(const name& contractID);
        // void unsigndon(const name& contractName, const name& drafterName);
    };

    namespace actions {
        using draftdon = eosio::action_wrapper<"draftdon"_h, &donations::draftdon, fixedProps::contract_account>;
        using signdon = eosio::action_wrapper<"signdon"_h, &donations::signdon, fixedProps::contract_account>;
    }

}  // namespace system_epn
