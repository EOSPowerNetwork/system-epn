#include "epn_test_chain.hpp"
#include "helpers.hpp"

using namespace system_epn;
using namespace system_epn::reservedNames;
using namespace system_epn::constants;
using namespace fixedProps;
using eosio::name;
using eosio::test_chain;
using std::vector;

namespace
{
    // TODO - remove these global objects once these authentication objects are moved to the public tester interface

    struct tester_permission_level_weight {
        eosio::permission_level permission = {};
        uint16_t weight = {};
    };
    EOSIO_REFLECT(tester_permission_level_weight, permission, weight);

    struct tester_wait_weight {
        uint32_t wait_sec = {};
        uint16_t weight = {};
    };
    EOSIO_REFLECT(tester_wait_weight, wait_sec, weight);

    struct tester_authority {
        uint32_t threshold = {};
        std::vector<eosio::key_weight> keys = {};
        std::vector<tester_permission_level_weight> accounts = {};
        std::vector<tester_wait_weight> waits = {};
    };
    EOSIO_REFLECT(tester_authority, threshold, keys, accounts, waits);

    eosio::permission_level EPNauthority{contract_account, code_permission};

}  // namespace

/*  Ready to delete
test_chain::user_context epn_test_chain::setup_getPowerUser() {
    tester_authority power_user_auth{.threshold = 1, .keys = {{test_chain::default_pub_key, 1}}, .accounts = {{{EPNauthority, 1}}}};
    auto user{"ethan"_n};
    // clang-format off
    chain.transact({action{{{"eosio"_n, "active"_n}},
                        "eosio"_n,
                        "newaccount"_n,
                        std::make_tuple("eosio"_n, user, power_user_auth, power_user_auth)}},
                nullptr);
    // clang-format on

    return chain.as("ethan"_n);
}
*/

epn_test_chain::epn_test_chain(const vector<name>& regularUsers, const vector<name>& powerUsers) {
    allUsers.insert(allUsers.end(), regularUsers.begin(), regularUsers.end());
    allUsers.insert(allUsers.end(), powerUsers.begin(), powerUsers.end());

    // Install standard system contracts
    chain.create_code_account(token_contract_account);
    chain.set_code("eosio.token"_n, CLSDK_CONTRACTS_DIR "token.wasm");

    // Install main system contract accounts
    chain.create_code_account(contract_account);
    chain.set_code(contract_account, "artifacts/system_epn.wasm");
    chain.create_account(revenue_account);  // Todo - Eventually the revenue account will be a contract

    // Create user accounts
    setup_createUserAccounts(regularUsers);
    setup_createPowerUserAccounts(powerUsers);

    // Setup tokens
    setup_system_token();

    // Distribute tokens
    setup_fundUsers();
}

test_chain::user_context epn_test_chain::as(name n1) {
    return chain.as(n1);
}

std::array<test_chain::user_context, 2> epn_test_chain::as(name n1, name n2) {
    return {chain.as(n1), chain.as(n2)};
}

std::array<test_chain::user_context, 3> epn_test_chain::as(name n1, name n2, name n3) {
    return {chain.as(n1), chain.as(n2), chain.as(n3)};
}

std::array<test_chain::user_context, 4> epn_test_chain::as(name n1, name n2, name n3, name n4) {
    return {chain.as(n1), chain.as(n2), chain.as(n3), chain.as(n4)};
}

void epn_test_chain::setup_createUserAccounts(const std::vector<eosio::name>& regularUsers) {
    for (auto user : regularUsers) {
        chain.create_account(user);
    }
}

void epn_test_chain::setup_createPowerUserAccounts(const std::vector<eosio::name>& powerUsers) {
    tester_authority simple_auth{
        .threshold = 1,
        .keys = {{test_chain::default_pub_key, 1}},
    };

    tester_authority power_user_auth{.threshold = 1, .keys = {{test_chain::default_pub_key, 1}}, .accounts = {{{EPNauthority, 1}}}};

    for (auto user : powerUsers) {
        chain.transact({action{
                           {{"eosio"_n, "active"_n}},        // Auth
                           "eosio"_n,                        // Contract account
                           "newaccount"_n,                   // Action
                           std::make_tuple("eosio"_n,        // Creator
                                           user,             // New account name
                                           simple_auth,      // Owner authority
                                           power_user_auth)  // Active authority
                       }},
                       nullptr);
    }
}

void epn_test_chain::setup_setPowerUserAccount(test_chain::user_context& user) {
    check(user.level.size() > 0, "Should never happen. User doesn't have any permmissions.");

    tester_authority power_user_auth{.threshold = 1, .keys = {{test_chain::default_pub_key, 1}}, .accounts = {{{EPNauthority, 1}}}};

    eosio::name userName = user.level[0].actor;

    auto trace = user.t.transact({action{
                                     user.level,                       // Auth
                                     "eosio"_n,                        // Contract account
                                     "updateauth"_n,                   // Action
                                     std::make_tuple(userName,         // Account
                                                     "active"_n,       // Permission
                                                     "owner"_n,        // Parent permission
                                                     power_user_auth)  // New authority
                                 }},
                                 {test_chain::default_priv_key},  // Signed Keys
                                 nullptr                          // Expected exception
    );
    expect(trace, nullptr);
}

void epn_test_chain::setup_system_token() {
    chain.as(token_contract_account).act<token::actions::create>(system_account, constants::EOS_Supply);
    chain.as(system_account).act<token::actions::issue>(system_account, constants::EOS_Supply, "");
}

void epn_test_chain::setup_fundUsers() {
    for (auto user : allUsers) {
        chain.as("eosio"_n).act<token::actions::transfer>(system_account, user, constants::user_balance, "");
    }
}
