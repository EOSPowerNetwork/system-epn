#pragma once

#include <string_view>

namespace error {
    using std::string_view;
    constexpr string_view doubleDraft = "Contract already drafted";
    constexpr string_view missingAuth = "Missing required authority";
    constexpr string_view memoTooLong = "Memo too long";
    constexpr string_view frequencyOutOfRange = "Frequency out of range";
    constexpr string_view invalidSigner = "Signer not valid for ths contract";
    constexpr string_view invalidCurrency = "Token type not currently supported";
    constexpr string_view duplicateSigner = "Signer cannot sign twice";
    constexpr string_view contractDNE = "Contract does not exist";
    constexpr string_view invalidAssetData = "Asset data invalid";
    constexpr string_view missingPermission = "Provided keys, permissions, and delays do not satisfy declared authorizations";
}  // namespace error
