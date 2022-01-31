#include "Donations.hpp"

#include <eosio/check.hpp>

#include "core/fixedprops.hpp"
#include "core/errormessages.hpp"

namespace system_epn
{
    using eosio::check;
    //using eosio::block_timestamp;
    //using eosio::const_mem_fun;
    //using eosio::current_block_time;
    //using eosio::indexed_by;
    //using eosio::name;
    //using eosio::seconds;
    //using std::distance;
    //using std::find_if;
    //using std::string;
    //using std::vector;

    DonationContract::DonationContract(const name& drafter, const name& contractID) 
        : drafter(drafter)
        , contractID(contractID) {
            auto drafts = DrafterMIType(fixedProps::contract_account, drafter.value);
            check(drafts.find(contractID.value) != drafts.end(), error::contractDNE.data());
        }

    void DonationContract::sign(const name& signer, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo) {
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

    Memo DonationContract::getMemoSuffix() const {
        auto drafts = DrafterMIType(fixedProps::contract_account, drafter.value);
        auto iter = drafts.find(contractID.value);
        check(iter != drafts.end(), error::contractDNE.data());

        return iter->memoSuffix;
    }

//////////////////////////////
    
    void Donations::draft(const name& owner, const name& contractID, const Memo& memoSuffix) {
        auto drafts = DrafterMIType(fixedProps::contract_account, owner.value);
        check(drafts.find(contractID.value) == drafts.end(), error::doubleDraft.data());

        auto configureNewDraft = [&](auto& row) {
            row.contractID = contractID;
            row.memoSuffix = memoSuffix;
        };

        auto ram_payer = owner;
        drafts.emplace(ram_payer, configureNewDraft);  //Todo change back to ramPpayer
    }

    DonationContract Donations::getDonation(const name& drafter, const name& contractID) {
        return DonationContract(drafter, contractID);
    }

}  // namespace system_epn

