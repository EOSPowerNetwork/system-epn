#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>

#include "Donations.hpp"

namespace system_epn
{
    using std::string;
    using namespace eosio;
    using system_epn::Donations;

    extern const char* draftdon_ricardian;
    extern const char* signdon_ricardian;
    extern const char* ricardian_clause2;

    class donations : public contract
    {
       public:
        using contract::contract;

        void draftdon(const name& owner, const name& contractID, const Memo& memoSuffix, const bool drafterPaysSignerRAM);
        void signdon(const name& signer, const name& owner, const name& contractID, const asset& quantity, const Frequency& frequency, const Memo& signerMemo);

        // void deletedon(const name& contractID);
        // void unsigndon(const name& contractName, const name& drafterName);
        using MIType = eosio::multi_index<"donations"_n, Donations>;
        class DonationsTable : public MIType
        {
           public:
            DonationsTable(name code, uint64_t scope) : MIType(code, scope) {}
            void draft(const name& contractID, const Memo& memoSuffix, const bool drafterPaysSignerRAM)
            {
                check(find(contractID.value) == end(), error::doubleDraft.data());

                auto ram_payer = eosio::name(get_scope());
                emplace(ram_payer, [&](auto& row) {
                    row.contractID = contractID;
                    row.memoSuffix = memoSuffix;
                    row.drafterPaysSignerRAM = drafterPaysSignerRAM;
                });
            }
        };
    };

}  // namespace system_epn
