#include <tuple>

#include "core/fixedprops.hpp"

#include "exec.hpp"
#include "ricardians.hpp"

#define EXEC_ACTION(CLASS_NAME, ACTION_NAME)                                                                  \
    case eosio::hash_name(#ACTION_NAME):                                                                      \
        executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &CLASS_NAME::ACTION_NAME); \
        break;

namespace system_epn
{
    namespace actions
    {
        using contract_c1 = exec;

        using fixedProps::contract_account;

        template <typename F>
        void for_each_action(F&& f) {
            f("execute"_h, eosio::action_type_wrapper<&contract_c1::execute>{}, ricardians::execute_ricardian);
        }

        inline bool eosio_apply(uint64_t receiver, uint64_t code, uint64_t action) {
            bool executed = false;
            if (code == receiver) {
                switch (action) { EXEC_ACTION(exec, execute) }
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

// Final part of the dispatcher
EOSIO_ACTION_DISPATCHER(system_epn::actions)

// Things to populate the ABI with
EOSIO_ABIGEN(actions(system_epn::actions),
             //table("donsigners"_n, system_epn::DonationSignature),
             ricardian_clause("Execute clause", ricardians::ricardian_clause))
