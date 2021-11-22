#pragma once

#include <string_view>

namespace error
{
    using std::string_view;
    constexpr string_view doubleDraft = "Contract already drafted";
    constexpr string_view wrongOwner = "Missing required authority";
    constexpr string_view memoTooLong = "Memo too long";
}  // namespace error
