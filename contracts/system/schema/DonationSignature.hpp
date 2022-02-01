#pragma once

#include <eosio/name.hpp>
#include <eosio/operators.hpp>
#include <eosio/reflection.hpp>
#include <eosio/time.hpp>

#include "core/data_types/include/Asset.hpp"
#include "core/data_types/include/Frequency.hpp"
#include "core/data_types/include/Memo.hpp"

namespace system_epn
{
    struct DonationSignature {
        DonationSignature() = default;

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
        eosio::name signer;
        eosio::name contractID;
        eosio::block_timestamp serviceBlock;
        // Todo - I think if I make it so all donations need to be named uniquely,
        //    then I can get rid of drafter from the signer table.
        eosio::name drafter;
        system_epn::Asset quantity;
        system_epn::Frequency frequency;
        system_epn::Memo signerMemo;
    };
    EOSIO_REFLECT(DonationSignature, index, signer, contractID, serviceBlock, drafter, quantity, frequency, signerMemo);
    EOSIO_COMPARE(DonationSignature);
};  // namespace system_epn