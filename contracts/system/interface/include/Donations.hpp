#pragma once

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/system.hpp>

#include <string>
#include <vector>

#include "core/data_types/include/Asset.hpp"
#include "core/data_types/include/Frequency.hpp"
#include "core/data_types/include/Memo.hpp"

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
        // Todo - I think if I make it so all donations need to be named uniquely, 
        //    then I can get rid of drafter from the signer table.
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

    // Interface with donations tables in RAM more easily
    class DonationContract {
       public:
        DonationContract(const name& drafter, const name& contractID);

        void sign(const name& signer, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo);
        Memo getMemoSuffix() const;

       private:
        name drafter;
        name contractID;
    };

    class Donations {
       public:
        void draft(const name& owner, const name& contractID, const Memo& memoSuffix);
        DonationContract getDonation(const name& drafter, const name& contractID);
    };

}  // namespace system_epn

// Todo - I think if I make it so all donations need to be named uniquely, then I can get rid of drafter from the signer table.
