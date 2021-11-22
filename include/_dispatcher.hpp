#pragma once
#include <tuple>
#include "donations.hpp"
#include "paycontracts.hpp"

namespace system_epn
{
    static constexpr auto contract_account = "contractacc"_n;

    namespace actions
    {
        using contract_c1 = paycontracts;
        using contract_c2 = donations;

        using sayhi = eosio::action_wrapper<"sayhi"_h, &contract_c1::sayhi, contract_account>;
        using sayhialice = eosio::action_wrapper<"sayhialice"_h, &contract_c1::sayhialice, contract_account>;

        using draftdon = eosio::action_wrapper<"draftdon"_h, &contract_c2::draftdon, contract_account>;
        //using signdon = eosio::action_wrapper<"signdon"_h, &contract_c2::signdon, contract_account>;

        template <typename F>
        void for_each_action(F&& f)
        {
            f("sayhi"_h, eosio::action_type_wrapper<&contract_c1::sayhi>{}, sayhi_ricardian);
            f("sayhialice"_h, eosio::action_type_wrapper<&contract_c1::sayhialice>{}, sayhialice_ricardian, "someone");

            f("draftdon"_h, eosio::action_type_wrapper<&contract_c2::draftdon>{}, draftdon_ricardian, "owner", "contractID", "memoSuffix");
            //f("signdon"_h, eosio::action_type_wrapper<&contract_c2::signdon>{}, signdon_ricardian, "to", "amount", "frequency");
        }

        inline bool eosio_apply(uint64_t receiver, uint64_t code, uint64_t action)
        {
            bool executed = false;
            if (code == receiver)
            {
                switch (action)
                {
                    case eosio::hash_name("sayhi"):
                        executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &paycontracts::sayhi);
                        break;
                    case eosio::hash_name("sayhialice"):
                        executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &paycontracts::sayhialice);
                        break;
                    case eosio::hash_name("draftdon"):
                        executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &donations::draftdon);
                        break;
                        // case eosio::hash_name("signdon"):
                        //     executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &donations::signdon);
                        //     break;
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
