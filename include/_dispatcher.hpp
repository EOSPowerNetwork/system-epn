#pragma once
#include <tuple>
#include "donations.hpp"
#include "paycontracts.hpp"

#define EXEC_ACTION(CLASS_NAME, ACTION_NAME)                                                                  \
    case eosio::hash_name(#ACTION_NAME):                                                                      \
        executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &CLASS_NAME::ACTION_NAME); \
        break;

namespace system_epn {
    static constexpr auto contract_account = "system.epn"_n;

    namespace actions {
        using contract_c1 = paycontracts;
        using contract_c2 = donations;

        using addasset = eosio::action_wrapper<"addasset"_h, &contract_c1::addasset, contract_account>;

        using draftdon = eosio::action_wrapper<"draftdon"_h, &contract_c2::draftdon, contract_account>;
        using signdon = eosio::action_wrapper<"signdon"_h, &contract_c2::signdon, contract_account>;

        template <typename F>
        void for_each_action(F&& f)
        {
            f("addasset"_h, eosio::action_type_wrapper<&contract_c1::addasset>{}, addasset_ricardian, "type", "contract", "minAmount", "maxAmount");

            f("draftdon"_h, eosio::action_type_wrapper<&contract_c2::draftdon>{}, draftdon_ricardian, "owner", "contractID", "memoSuffix");
            f("signdon"_h, eosio::action_type_wrapper<&contract_c2::signdon>{}, signdon_ricardian, "signer", "drafter", "contractID", "quantity", "frequency", "signerMemo");
        }

        inline bool eosio_apply(uint64_t receiver, uint64_t code, uint64_t action)
        {
            bool executed = false;
            if (code == receiver) {
                switch (action) {
                    EXEC_ACTION(paycontracts, addasset)
                    EXEC_ACTION(donations, draftdon)
                    EXEC_ACTION(donations, signdon)
                }
                eosio::check(executed == true, "unknown action");
            }
            return executed;
        }

        /*  Would be nice if there was a simple clsdk dispatcher helper 
         *  macro for multi-class contracts, for example:

            EOSIO_ACTIONS("contractname"_n, (
                paycontracts,
                action(sayhi, name, ricardian_contract(sayhi_ricardian))
            ),(
                donations,
                action(saybye, name, ricardian_contract(saybye_ricardian))
            ))
        */

    }  // namespace actions

}  // namespace system_epn
