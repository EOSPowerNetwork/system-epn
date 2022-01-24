#pragma once

#include <eosio/tester.hpp>
#include <vector>

namespace system_epn
{
    class epn_test_chain {
       public:
        epn_test_chain(const std::vector<eosio::name>& regularUsers, const std::vector<eosio::name>& powerUsers);

        void setup_setPowerUserAccount(eosio::test_chain::user_context& user);

        eosio::test_chain::user_context as(eosio::name);
        std::array<eosio::test_chain::user_context, 3> as(eosio::name, eosio::name, eosio::name);
        std::array<eosio::test_chain::user_context, 2> as(eosio::name, eosio::name);
        std::array<eosio::test_chain::user_context, 4> as(eosio::name, eosio::name, eosio::name, eosio::name);

        eosio::test_chain& getChain() {
            return chain;
        }

       private:
        eosio::test_chain chain;
        std::vector<eosio::name> allUsers;

        void setup_installMySystemContracts();
        void setup_createUserAccounts(const std::vector<eosio::name>& regularUsers);
        void setup_createPowerUserAccounts(const std::vector<eosio::name>& powerUsers);
        void setup_system_token();
        void setup_fundUsers();

        eosio::transaction_trace set_abi(eosio::name ac, const char* filename, const char* expected_except = nullptr);
    };
}  // namespace system_epn