#include <eosio/tester.hpp>
#include <token/token.hpp>
#include "_dispatcher.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace eosio;

// Setup function to install my contract to the chain
void setup_installMyContract(test_chain& t)
{
    t.create_code_account(system_epn::contract_account);
    t.set_code(system_epn::contract_account, "system_epn.wasm");
}

// Setup function to add some accounts to the chain
void setup_createAccounts(test_chain& t)
{
    for (auto user : {"alice"_n, "bob"_n, "charlie"_n, "dan"_n})
    {
        t.create_account(user);
    }
}

// Sample setup function to set up the eosio.token contract
void setup_EOS_token(test_chain& t)
{
    t.create_code_account("eosio.token"_n);
    t.set_code("eosio.token"_n, CLSDK_CONTRACTS_DIR "token.wasm");

    t.as("eosio.token"_n).act<token::actions::create>("eosio"_n, s2a("1000000.0000 EOS"));
    t.as("eosio"_n).act<token::actions::issue>("eosio"_n, s2a("1000000.0000 EOS"), "");
}

void setup_hacker_EOS_token(test_chain& t)
{
    // Set up a hacker token contract to ensure I'm correctly processing notification handlers
    // One of the arguments to EOSIO_ACTIONS is a default contract. EOSIO_ACTIONS places the default
    // contract in the action wrappers. act<...> and trace<...> normally send the action to the
    // default. with_code() overrides that.
    t.create_code_account("hacker.token"_n);
    t.set_code("hacker.token"_n, CLSDK_CONTRACTS_DIR "token.wasm");
    t.as("hacker.token"_n).with_code("hacker.token"_n).act<token::actions::create>("hacker.token"_n, s2a("1000000.0000 EOS"));
    t.as("hacker.token"_n).with_code("hacker.token"_n).act<token::actions::issue>("hacker.token"_n, s2a("1000000.0000 EOS"), "");
}

// Sample setup function to fund some users
void setup_fundUsers(test_chain& t)
{
    for (auto user : {"alice"_n, "bob"_n, "jane"_n, "joe"_n})
    {
        t.as("eosio"_n).act<token::actions::transfer>("eosio"_n, user, s2a("10000.0000 EOS"), "");
        t.as("eosio"_n).act<token::actions::transfer>("eosio"_n, user, s2a("10000.0000 OTHER"), "");
        t.as("hacker.token"_n).with_code("hacker.token"_n).act<token::actions::transfer>("hacker.token"_n, user, s2a("10000.0000 EOS"), "");
    }
}

test_chain::user_context getAcc(test_chain& t)
{
    return t.as("alice"_n);
}

std::pair<test_chain::user_context, test_chain::user_context> get2Acc(test_chain& t)
{
    return {t.as("alice"_n), t.as("bob"_n)};
}

void setupChain(test_chain& t)
{
    setup_installMyContract(t);
    setup_createAccounts(t);
}