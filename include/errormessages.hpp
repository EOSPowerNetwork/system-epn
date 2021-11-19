#pragma once

#include <string_view>

namespace error
{
    using std::string_view;
    namespace donations
    {
        constexpr string_view doubledraft = "Donation already drafted";
    }
}  // namespace error
