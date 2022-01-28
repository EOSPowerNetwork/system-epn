#include <eosio/check.hpp>
#include <eosio/action.hpp>
#include <eosio/abi.hpp>
#include <eosio/from_json.hpp>

#include "core/fixedprops.hpp"

#include "epn_test_chain.hpp"
#include "helpers.hpp"

using namespace system_epn;
using namespace system_epn::reservedNames;
using namespace system_epn::constants;
using namespace fixedProps;
using eosio::name;
using eosio::test_chain;
using eosio::transaction_trace;
using eosio::read_whole_file;
using eosio::json_token_stream;
using eosio::action;
using eosio::check;
using std::vector;

/*
    kill nodeos: 
        kill `ps -A | grep [n]odeos | awk '{print $1}'
*/


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
    set_abi("eosio.token"_n, CLSDK_CONTRACTS_DIR "token.abi");
    
    // Install main system contract accounts
    chain.create_code_account(contract_account);
    chain.set_code(contract_account, "artifacts/system.wasm");
    set_abi(contract_account, "artifacts/system.abi");
    chain.create_account(revenue_account);  // Todo - Eventually this account will be a contract
    chain.create_account(exec_account);     // Todo - Eventually this account will be a contract

    // Create user accounts
    setup_createUserAccounts(regularUsers);
    setup_createPowerUserAccounts(powerUsers);

    // Setup tokens
    setup_system_token();

    // Distribute tokens
    setup_fundUsers();
}

transaction_trace epn_test_chain::set_abi(name ac, const char* filename, const char* expected_except)
{
    eosio::abi_def abi;
    auto abiFile = read_whole_file(filename);
    auto abiString = std::string(abiFile.begin(), abiFile.end());
    json_token_stream stream(abiString.data());
    eosio::from_json(abi, stream);

    return chain.transact({action{{{ac, "active"_n}},
                        "eosio"_n,
                        "setabi"_n,
                        std::make_tuple(ac, eosio::pack(abi))}},
                expected_except);
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

int32_t epn_test_chain::launch_nodeos() {
    
    // Make all prior transactions irreversible. This causes the transactions to
    // go into the block log.
    chain.finish_block();
    chain.finish_block();

    // Copy blocks.log into a fresh directory for nodeos to use
    eosio::execute("rm -rf example_chain");
    eosio::execute("mkdir -p example_chain/blocks");
    eosio::execute("cp " + chain.get_path() + "/blocks/blocks.log example_chain/blocks");

    // Run nodeos
    int32_t ret = 0;
    ret = eosio::execute(
        "nodeos -d example_chain "
        "--config-dir example_config "
        "--plugin eosio::chain_plugin "
        "--plugin eosio::chain_api_plugin "
        "--plugin eosio::history_plugin "
        "--plugin eosio::history_api_plugin "
        "--plugin eosio::producer_plugin "
        "--plugin eosio::http_plugin --verbose-http-errors "
        "--plugin eosio::epn_plugin --permission \"active\" --operator-name \"eosio\" --signing-private-key \"5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3\" "
        "--access-control-allow-origin \"*\" "
        "--access-control-allow-header \"*\" "
        "--http-validate-host 0 "
        "--http-server-address 0.0.0.0:8888 "
        "--contracts-console "
        "-e -p eosio");

    return ret;
}