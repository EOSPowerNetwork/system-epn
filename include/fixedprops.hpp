#pragma once

#include <array>
#include <eosio/asset.hpp>
#include <eosio/name.hpp>
#include <string_view>

// Do not modify once contract is live
namespace fixedProps
{
    static constexpr auto contract_account = "system.epn"_n;
    static constexpr auto revenue_account = "revenue.epn"_n;

    namespace memo
    {
        using std::string_view;

        // Memo of signer paying a drafter looks like:
        // [EOS Power Network] <SignerSpecifiedMemo> -- <DrafterSpecificMemo>
        // == marketingMemo + signerMemo + delimiter + drafterMemo
        constexpr string_view memoDelimiter = " -- ";
        constexpr string_view marketingMemo = "[EOS Power Network] ";
        constexpr string_view feeMemo = "Transaction fee";

        // Marketing memo and memo delimiter must have fixed size.
        // If they were to grow after an org/signer established their memos, the total size may exceed the 256 byte memo limit
        constexpr uint16_t totalMemoBytes = 256;
        constexpr uint16_t marketingMemoBytes = 20;
        constexpr uint16_t delimiterBytes = 4;
        constexpr uint16_t remainingBytes = totalMemoBytes - marketingMemoBytes - delimiterBytes;
        constexpr uint16_t memoSize = remainingBytes / 2;  // 116 bytes for each memo

        // Fee memo checks
        constexpr uint16_t feeMemoBytes = 15;
        static_assert(marketingMemoBytes + feeMemoBytes < totalMemoBytes);

        // Serviced transaction memo checks
        static_assert(memoDelimiter.size() == delimiterBytes);
        static_assert(marketingMemo.size() == marketingMemoBytes);
        static_assert(memoDelimiter.size() + marketingMemo.size() + 2 * memoSize == totalMemoBytes);
    }  // namespace memo

    namespace Frequency
    {
        constexpr uint32_t minimum_frequency_seconds = 23 * 60 * 60;        // 23 hours
        constexpr uint32_t maximum_frequency_seconds = 365 * 24 * 60 * 60;  // One year
    }                                                                       // namespace Frequency

    namespace Assets
    {
        using eosio::name;
        using eosio::symbol;
        using std::array;
        using std::find_if;

        constexpr float transactionFee = 0.03;

        struct AssetProps {
            AssetProps() = default;
            constexpr AssetProps(symbol sym, name contract, int64_t min, int64_t max)
                : sym(sym)
                , contract(contract)
                , minimum(min)
                , maximum(max) {}
            eosio::symbol sym;
            eosio::name contract;
            int64_t minimum;
            int64_t maximum;
        };

        constexpr uint64_t precision = 4;
        constexpr int64_t EOS = 1e4;
        constexpr array<AssetProps, 1> supportedTokens = {AssetProps(symbol("EOS", precision), "eosio.token"_n, 1 * EOS, 1000 * EOS)};

        constexpr std::optional<AssetProps> getAssetProps(const eosio::symbol_code& sym) {
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
