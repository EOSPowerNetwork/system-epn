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
             - The purposes of these interfaces are very ambiguous. They almost do the same exact thing. Either refactor
                into one interface, or figure out a more sensible design.
    */

    // Interface with donations tables in RAM more easily
    class DonationContract {
       public:
        DonationContract(const eosio::name& drafter, const eosio::name& contractID);

        static bool exists(const eosio::name& drafter, const eosio::name& contractID);

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

    struct DonationsIntf {
        static void draft(const eosio::name& owner, const eosio::name& contractID, const Memo& memoSuffix);

        static size_t getNumSigners(const eosio::name& drafter, const eosio::name& contractID);

        static DonationContract getDonation(const eosio::name& drafter, const eosio::name& contractID);

        static DonationSignature getSignature(const eosio::name& drafter, const eosio::name& contractID, const eosio::name& signer);

        static std::vector<DonationSignature> getAllSignatures();

        static bool exists(const eosio::name& drafter, const eosio::name& contractID);
    };

}  // namespace system_epn
