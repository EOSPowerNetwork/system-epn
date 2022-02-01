#pragma once

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/system.hpp>

#include <string>
#include <vector>

#include "core/data_types/include/Asset.hpp"
#include "core/data_types/include/Frequency.hpp"
#include "core/data_types/include/Memo.hpp"

#include "schema/DonationDraft.hpp"
#include "schema/DonationSignature.hpp"

namespace system_epn
{
    /*
        Todo - I think if I make it so all donations need to be named uniquely, then I can get rid of drafter 
                from the signer table and simplify all interfaces to just query by contractID
             - Rename this file to something less similar to "donations.hpp" (the contract)
    */

    // Interface with donations tables in RAM more easily
    class DonationsIntf {
       public:
        DonationsIntf(const eosio::name& drafter, const eosio::name& contractID);

        static void draft(const eosio::name& owner, const eosio::name& contractID, const Memo& memoSuffix);
        static bool exists(const eosio::name& drafter, const eosio::name& contractID);
        static std::vector<DonationSignature> getAllContractSignatures();

        void sign(const eosio::name& signer, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo);
        Memo getMemoSuffix() const;
        size_t getNumSigners() const;
        DonationSignature getSignature(const eosio::name& signer) const;

       private:
        eosio::name drafter;
        eosio::name contractID;

        std::vector<DonationSignature> _signatures;
        DonationDraft _draft;
    };

}  // namespace system_epn
