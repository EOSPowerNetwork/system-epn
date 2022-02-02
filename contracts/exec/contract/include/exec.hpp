#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>
#include <string>
#include <vector>

#include "core/fixedprops.hpp"

namespace system_epn
{
    class exec : public eosio::contract {
       public:
        using eosio::contract::contract;

        exec(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds);

        void execute();
    };

    namespace actions
    {
        using execute = eosio::action_wrapper<"execute"_h, &exec::execute, fixedProps::contract_account>;
    }
}  // namespace system_epn