#pragma once

#include <eosio/name.hpp>
#include <eosio/reflection.hpp>

#include "core/data_types/include/Memo.hpp"

namespace system_epn
{
    struct DonationDraft {
        // This table is scoped to the owner, so owner does not need to be part of the table
        eosio::name contractID;
        system_epn::Memo memoSuffix;

        uint64_t primary_key() const {
            return contractID.value;
        }
    };
    EOSIO_REFLECT(DonationDraft, contractID, memoSuffix);
};  // namespace system_epn