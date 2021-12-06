#pragma once

#include <eosio/eosio.hpp>

namespace system_epn {
    namespace ramConsumption_bytes {
        namespace firstEmplace {
            constexpr int64_t DraftDonation = 238;
        }  // namespace firstEmplace
        namespace subsequentEmplace {
            constexpr int64_t DraftDonation = 126;
        }  // namespace subsequentEmplace
        namespace modify {
            namespace oldPayer {
                constexpr int64_t SignDonation = -126;
            }
            namespace newPayer {
                constexpr int64_t SignDonation = 166;
            }
        }  // namespace modify
    }      // namespace ramConsumption_bytes
}  // namespace system_epn