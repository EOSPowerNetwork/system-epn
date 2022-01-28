#pragma once

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/system.hpp>

#include <string>
#include <vector>

#include "core/fixedprops.hpp"
#include "core/schema/Asset.hpp"
#include "core/schema/Frequency.hpp"
#include "core/schema/Memo.hpp"

namespace system_epn
{
    using eosio::block_timestamp;
    using eosio::const_mem_fun;
    using eosio::current_block_time;
    using eosio::indexed_by;
    using eosio::name;
    using eosio::seconds;
    using std::distance;
    using std::find_if;
    using std::string;
    using std::vector;

    struct SignerData {
        SignerData() = default;

        uint64_t primary_key() const {
            return index;
        }

        uint64_t get_secondary_1() const {
            return signer.value;
        }

        uint64_t get_secondary_2() const {
            return contractID.value;
        }

        uint64_t get_secondary_3() const {
            return serviceBlock.to_time_point().elapsed.count();
        }

        uint64_t index;
        name signer;
        name contractID;
        block_timestamp serviceBlock;
        name drafter;
        Asset quantity;
        Frequency frequency;
        Memo signerMemo;
    };
    EOSIO_REFLECT(SignerData, index, signer, contractID, serviceBlock, drafter, quantity, frequency, signerMemo);
    EOSIO_COMPARE(SignerData);
    using SignerMIType = eosio::multi_index<"signers"_n,
                                            SignerData,
                                            indexed_by<"bysigner"_n, const_mem_fun<SignerData, uint64_t, &SignerData::get_secondary_1>>,
                                            indexed_by<"bycontractid"_n, const_mem_fun<SignerData, uint64_t, &SignerData::get_secondary_2>>,
                                            indexed_by<"byservblock"_n, const_mem_fun<SignerData, uint64_t, &SignerData::get_secondary_3>>>;

    struct DrafterData {
        // This table is scoped to the owner, so owner does not need to be part of the table
        name contractID;
        Memo memoSuffix;

        uint64_t primary_key() const {
            return contractID.value;
        }
    };
    EOSIO_REFLECT(DrafterData, contractID, memoSuffix);
    using DrafterMIType = eosio::multi_index<"drafts"_n, DrafterData>;

    // Wrapper class to make interacting with the donations table in RAM easier.
    class DonationContract /*: SignerMIType*/ {
       public:
        DonationContract(const name& drafter, const name& contractID)
            //: SignerMIType(fixedProps::contract_account, fixedProps::contract_account.value)
            : drafter(drafter)
            , contractID(contractID) {
            auto drafts = DrafterMIType(fixedProps::contract_account, drafter.value);
            check(drafts.find(contractID.value) != drafts.end(), error::contractDNE.data());
        }

        void sign(const name& signer, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo) {
            check(signer != drafter, error::invalidSigner.data());
            SignerMIType signatures(fixedProps::contract_account, fixedProps::contract_account.value);

            auto sigsBySigner = signatures.get_index<"bysigner"_n>();
            auto feq = [&](const SignerData& row) { return row.contractID == contractID && row.drafter == drafter; };
            auto itr = find_if(sigsBySigner.lower_bound(signer.value), sigsBySigner.upper_bound(signer.value), feq);
            check(itr == sigsBySigner.upper_bound(signer.value), error::duplicateSigner.data());

            // Calculate next block timestamp where we should service the pull transaction
            auto timestamp = current_block_time();
            auto nextTimestamp = block_timestamp(timestamp.to_time_point() + seconds(frequency.value));

            uint64_t index = static_cast<uint64_t>(distance(signatures.begin(), signatures.end()));
            auto addSigner = [&](SignerData& row) {
                row.index = index;
                row.signer = signer;
                row.contractID = contractID;
                row.serviceBlock = nextTimestamp;
                row.drafter = drafter;
                row.quantity = quantity;
                row.frequency = frequency;
                row.signerMemo = signerMemo;
            };

            const auto ram_payer = signer;
            signatures.emplace(ram_payer, addSigner);
        }

        Memo getMemoSuffix() const {
            auto drafts = DrafterMIType(fixedProps::contract_account, drafter.value);
            auto iter = drafts.find(contractID.value);
            check(iter != drafts.end(), error::contractDNE.data());

            return iter->memoSuffix;
        }

       private:
        name drafter;
        name contractID;
    };

    class Donations {
       public:
        void draft(const name& owner, const name& contractID, const Memo& memoSuffix) {
            auto drafts = DrafterMIType(fixedProps::contract_account, owner.value);
            check(drafts.find(contractID.value) == drafts.end(), error::doubleDraft.data());

            auto configureNewDraft = [&](auto& row) {
                row.contractID = contractID;
                row.memoSuffix = memoSuffix;
            };

            auto ram_payer = owner;
            drafts.emplace(ram_payer, configureNewDraft);  //Todo change back to ramPpayer
        }

        DonationContract getDonation(const name& drafter, const name& contractID) {
            return DonationContract(drafter, contractID);
        }
    };

}  // namespace system_epn

// Todo - I think if I make it so all donations need to be named uniquely, then I can get rid of drafter from the signer table.
