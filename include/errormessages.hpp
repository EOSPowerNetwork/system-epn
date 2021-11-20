#pragma once

#include <string_view>

namespace error
{
    using std::string_view;
    constexpr string_view doubleDraft = "Contract already drafted";
    constexpr string_view wrongOwner = "Owner must be a signer";
}  // namespace error
