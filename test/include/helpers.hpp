#pragma once

#include <eosio/tester.hpp>
#include <string_view>
#include <token/token.hpp>
#include <vector>
#include "_dispatcher.hpp"

using namespace eosio;
using std::vector;
using system_epn::Frequency;

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

eosio::permission_level EPNauthority{fixedProps::contract_account, "eosio.code"_n};

namespace testData
{
    Frequency freq_23Hours{23 * 60 * 60};
}

// Setup function to install my contract to the chain
void setup_installMySystemContracts(test_chain& t) {
    t.create_code_account(fixedProps::contract_account);
    t.set_code(fixedProps::contract_account, "artifacts/system_epn.wasm");

    // Todo - Eventually the revenue account will be a contract
    t.create_account(fixedProps::revenue_account);
}

// Setup function to add some accounts to the chain
void setup_createUserAccounts(test_chain& t) {
    for (auto user : {"alice"_n, "bob"_n, "charlie"_n, "dan"_n}) {
        t.create_account(user);
    }
}

void setup_createPowerUserAccounts(test_chain& t) {
    tester_authority simple_auth{
        .threshold = 1,
        .keys = {{test_chain::default_pub_key, 1}},
    };

    tester_authority power_user_auth{.threshold = 1, .keys = {{test_chain::default_pub_key, 1}}, .accounts = {{{EPNauthority, 1}}}};

    for (auto user : {"alice"_n, "bob"_n, "charlie"_n, "dan"_n}) {
        t.transact({action{
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

void setup_setPowerUserAccount(test_chain::user_context& user) {
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

test_chain::user_context setup_getPowerUser(test_chain& t) {
    tester_authority power_user_auth{.threshold = 1, .keys = {{test_chain::default_pub_key, 1}}, .accounts = {{{EPNauthority, 1}}}};
    auto user{"ethan"_n};
    // clang-format off
    t.transact({action{{{"eosio"_n, "active"_n}},
                        "eosio"_n,
                        "newaccount"_n,
                        std::make_tuple("eosio"_n, user, power_user_auth, power_user_auth)}},
                nullptr);
    // clang-format on

    return t.as("ethan"_n);
}

// Sample setup function to set up the eosio.token contract
void setup_EOS_token(test_chain& t) {
    t.create_code_account("eosio.token"_n);
    t.set_code("eosio.token"_n, CLSDK_CONTRACTS_DIR "token.wasm");

    t.as("eosio.token"_n).act<token::actions::create>("eosio"_n, s2a("1000000.0000 EOS"));
    t.as("eosio"_n).act<token::actions::issue>("eosio"_n, s2a("1000000.0000 EOS"), "");
}

void setup_hacker_EOS_token(test_chain& t) {
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
void setup_fundUsers(test_chain& t) {
    for (auto user : {"alice"_n, "bob"_n, "charlie"_n, "dan"_n}) {
        t.as("eosio"_n).act<token::actions::transfer>("eosio"_n, user, s2a("10000.0000 EOS"), "");
        t.as("hacker.token"_n).with_code("hacker.token"_n).act<token::actions::transfer>("hacker.token"_n, user, s2a("10000.0000 EOS"), "");
    }
}

void setup_fundUser(test_chain& t, eosio::name user, eosio::asset amount) {
    t.as("eosio"_n).act<token::actions::transfer>("eosio"_n, user, amount, "");
}

test_chain::user_context getAcc(test_chain& t) {
    return t.as("alice"_n);
}

std::pair<test_chain::user_context, test_chain::user_context> get2Acc(test_chain& t) {
    return {t.as("alice"_n), t.as("bob"_n)};
}

auto get3Acc(test_chain& t) {
    //{"alice"_n, "bob"_n, "charlie"_n, "dan"_n}
    return std::make_tuple(t.as("alice"_n), t.as("bob"_n), t.as("charlie"_n));
}

void setupChain(test_chain& t) {
    setup_installMySystemContracts(t);
    setup_createUserAccounts(t);
}

vector<ship_protocol::account_delta> getFirstRamDeltaSummary(const transaction_trace& trace) {
    const vector<action_trace>& actions = trace.action_traces;
    check(actions.size() != 0, "Can't return a ram delta for a trace with no actions!");

    return actions.at(0).account_ram_deltas;
}

int64_t getRamDelta(const ship_protocol::account_delta& account_delta, const name& n) {
    check(account_delta.account == n, "Wrong account");
    return account_delta.delta;
}

void printRamDeltas(const transaction_trace& trace) {
    const vector<action_trace>& actions = trace.action_traces;
    cout << "\n";
    for (size_t i = 0; i < actions.size(); ++i) {
        const auto& action = actions[i];
        const auto& ramDeltas = action.account_ram_deltas;
        cout << "\nAction " << i << ":\n";
        for (const auto& ramDelta : ramDeltas) {
            cout << "RAM Deltas:\n";
            cout << "Account: " << ramDelta.account << ", Delta: " << ramDelta.delta << "\n";
        }
        cout << "\n";
    }
}

void dprint(std::string s) {
    printf("\n%s\n", s.c_str());
}

bool succeeded(const transaction_trace& trace) {
    return (trace.status == transaction_status::executed);
}

bool failedWith(const transaction_trace& trace, std::string_view err) {
    return (trace.except->find(err.data()) != std::string::npos);
}

void dump_donations(const name& scope) {
    using namespace system_epn;
    using std::for_each;
    using std::to_string;

    printf("\n ========= %s Donations ========= \n", scope.to_string().c_str());
    system_epn::DrafterMIType _drafts(fixedProps::contract_account, scope.value);

    printf("%-12s %-12s %-12s %-12s\n", "DRAFTER", "CONTRACT ID", "SIGNER", "QUANTITY");
    for (auto& row : _drafts) {
        auto ID = row.contractID;
        printf("%-12s %-12s\n", ID.to_string().c_str(), scope.to_string().c_str());
        SignerMIType _signatures(fixedProps::contract_account, fixedProps::contract_account.value);
        auto byContractID = _signatures.get_index<"bycontractid"_n>();
        for_each(byContractID.lower_bound(ID.value), byContractID.upper_bound(ID.value), [&](const system_epn::SignerData& s) {
            if (s.drafter == scope) {
                printf("%-12s %-12s %-12s %-12s\n", " ", " ", s.signer.to_string().c_str(), to_string(s.quantity.value.amount).c_str());
            }
        });
        printf("\n");
    }
}

int64_t toMicroseconds(Frequency freq) {
    constexpr int64_t umPerSecond = 1e6;
    return static_cast<int64_t>(freq.value) * umPerSecond;
}
