#pragma once

#include <eosio/tester.hpp>
#include <string_view>
#include <token/token.hpp>
#include <vector>

namespace system_epn
{
    using eosio::name;

    using eosio::transaction_trace;

    namespace constants
    {
        const eosio::asset EOS_Supply = eosio::s2a("1000000.0000 EOS");
        const eosio::asset user_balance = eosio::s2a("10000.0000 EOS");
    }  // namespace constants

    namespace reservedNames
    {
        constexpr auto system_account = "eosio"_n;
        constexpr auto token_contract_account = "eosio.token"_n;
        constexpr auto code_permission = "eosio.code"_n;
    }  // namespace reservedNames

    namespace checks
    {
        bool succeeded(const eosio::transaction_trace& trace);
        bool failedWith(const transaction_trace& trace, std::string_view err);
    }  // namespace checks

    namespace debug
    {
        std::vector<eosio::ship_protocol::account_delta> getFirstRamDeltaSummary(const transaction_trace& trace);
        int64_t getRamDelta(const eosio::ship_protocol::account_delta& account_delta, const name& n);
        void printRamDeltas(const transaction_trace& trace);
        void dprint(std::string s);
        void dump_donations(const name& scope);
    }  // namespace debug

}  // namespace system_epn