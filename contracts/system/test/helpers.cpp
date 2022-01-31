#include <vector>

#include "system/interface/include/Donations.hpp"
#include "core/fixedprops.hpp"

#include "helpers.hpp"

using namespace std;
using namespace eosio;
using namespace system_epn;

using eosio::test_chain;
using std::array;

bool checks::succeeded(const transaction_trace& trace) {
    return (trace.status == transaction_status::executed);
}

bool checks::failedWith(const transaction_trace& trace, std::string_view err) {
    return (trace.except->find(err.data()) != std::string::npos);
}

vector<eosio::ship_protocol::account_delta> debug::getFirstRamDeltaSummary(const transaction_trace& trace) {
    const vector<action_trace>& actions = trace.action_traces;
    check(actions.size() != 0, "Can't return a ram delta for a trace with no actions!");

    return actions.at(0).account_ram_deltas;
}

int64_t debug::getRamDelta(const eosio::ship_protocol::account_delta& account_delta, const name& n) {
    check(account_delta.account == n, "Wrong account");
    return account_delta.delta;
}

void debug::printRamDeltas(const transaction_trace& trace) {
    const vector<eosio::action_trace>& actions = trace.action_traces;
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

void debug::dprint(std::string s) {
    printf("\n%s\n", s.c_str());
}

void debug::dump_donations(const name& scope) {
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
