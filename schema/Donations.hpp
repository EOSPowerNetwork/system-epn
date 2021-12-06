#pragma once

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>

#include <string>
#include <vector>

#include "Asset.hpp"
#include "Frequency.hpp"
#include "Memo.hpp"

namespace system_epn {
    using eosio::name;
    using std::string;
    using std::vector;

    struct SignerData {
        SignerData() = default;
        SignerData(name n, Asset a, Frequency f, Memo m)
            : signer(n)
            , quantity(a)
            , frequency(f)
            , signerMemo(m)
        {
        }

        bool operator==(const name& val) const
        {
            return val == signer;
        }

        name signer;
        Asset quantity;
        Frequency frequency;
        Memo signerMemo;
    };
    EOSIO_REFLECT(SignerData, signer, quantity, frequency, signerMemo);
    EOSIO_COMPARE(SignerData);

    // This table is scoped to the owner, so owner does not need to be part of the table
    struct Donation {
        name contractID;  // 64 bits
        Memo memoSuffix;  //
        vector<SignerData> signerData;

        uint64_t primary_key() const
        {
            return contractID.value;
        }
    };
    EOSIO_REFLECT(Donation, contractID, memoSuffix, signerData);

    // Wrapper class to make interacting with the donations table in RAM easier.
    using MIType = eosio::multi_index<"donations"_n, Donation>;
    class DonationsTable : public MIType {
       public:
        DonationsTable(name code, uint64_t scope)
            : MIType(code, scope)
        {
        }
        void draft(const name& contractID, const Memo& memoSuffix)
        {
            check(find(contractID.value) == end(), error::doubleDraft.data());
            auto ram_payer = eosio::name(get_scope());
            auto configureNewDraft = [&](auto& row) {
                row.contractID = contractID;
                row.memoSuffix = memoSuffix;
            };
            emplace(ram_payer, configureNewDraft);  //Todo change back to ramPpayer
        }

        void sign(const name& signer, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo)
        {
            check(get_scope() != signer.value, error::invalidSigner.data());
            auto donation = require_find(contractID.value, error::contractDNE.data());

            const auto& sd = donation->signerData;

            bool duplicateSigner = (std::find(sd.cbegin(), sd.cend(), signer) != sd.end());
            check(!duplicateSigner, error::duplicateSigner);

            // Todo - Signers list should always be sorted
            auto addSigner = [&](Donation& row) {
                row.signerData.reserve(row.signerData.size() + 1);
                SignerData newSigner{signer, quantity, frequency, signerMemo};
                row.signerData.emplace_back(std::move(newSigner));
            };

            modify(donation, signer, addSigner);
        }
    };
}  // namespace system_epn