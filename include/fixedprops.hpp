#pragma once

#include <array>
#include <eosio/asset.hpp>
#include <eosio/name.hpp>
#include <string_view>

// Do not modify once contract is live
namespace fixedProps {
    namespace memo {
        using std::string_view;

        constexpr string_view memoDelimiter = " -- ";
        constexpr string_view marketingMemo = "[EOS Power Network] ";

        constexpr uint16_t totalMemoBytes = 256;
        constexpr uint16_t marketingMemoBytes = 20;
        constexpr uint16_t delimiterBytes = 4;
        constexpr uint16_t remainingBytes = totalMemoBytes - marketingMemoBytes - delimiterBytes;
        constexpr uint16_t memoSize = remainingBytes / 2;  // 116 bytes for each memo

        // Marketing memo and memo delimiter must have fixed size.
        // If they were to grow after an org/signer established their memos, the total size may exceed the 256 byte memo limit
        static_assert(memoDelimiter.size() == delimiterBytes);
        static_assert(marketingMemo.size() == marketingMemoBytes);
        static_assert(memoDelimiter.size() + marketingMemo.size() + 2 * memoSize == totalMemoBytes);
    }  // namespace memo

    namespace Frequency {
        constexpr uint32_t minimum_frequency_seconds = 23 * 60 * 60;        // 23 hours
        constexpr uint32_t maximum_frequency_seconds = 365 * 24 * 60 * 60;  // One year
    }                                                                       // namespace Frequency

    namespace Assets {
        using eosio::name;
        using eosio::symbol;
        using std::array;
        using std::find_if;

        struct AssetProps {
            AssetProps() = default;
            constexpr AssetProps(symbol sym, name contract, int64_t min, int64_t max)
                : sym(sym)
                , contract(contract)
                , minimum(min)
                , maximum(max)
            {
            }
            eosio::symbol sym;
            eosio::name contract;
            int64_t minimum;
            int64_t maximum;
        };

        // Constexpr power function
        // Credit: https://prosepoetrycode.potterpcs.net/2015/07/a-simple-constexpr-power-function-c/
        constexpr int64_t ipow(int64_t num, unsigned int pow)
        {
            return (pow >= sizeof(unsigned int) * 8) ? 0 : pow == 0 ? 1 : num * ipow(num, pow - 1);
        }

        constexpr uint64_t precision = 4;
        constexpr int64_t EOS = ipow(10, precision);
        constexpr array<AssetProps, 1> supportedTokens = {AssetProps(symbol("EOS", precision), "eosio.token"_n, 1 * EOS, 1000 * EOS)};

        constexpr std::optional<AssetProps> getAssetProps(const eosio::symbol_code& sym)
        {
            auto& s = supportedTokens;
            auto DoSymbolsMatch = [&](const AssetProps& props) {
                bool match = (props.sym.code() == sym);
                return match;
            };

            auto iter = find_if(s.begin(), s.end(), DoSymbolsMatch);
            if (iter != s.end()) {
                return {(*iter)};
            }
            else {
                return std::nullopt;
            }
        };

    };  // namespace Assets
};      // namespace fixedProps
