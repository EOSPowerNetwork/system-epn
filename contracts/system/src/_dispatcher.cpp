#include <tuple>

#include "core/fixedprops.hpp"
#include "interface/include/Donations.hpp"

#include "ricardians.hpp"
#include "donations.hpp"
#include "paycontracts.hpp"

#define EXEC_ACTION(CLASS_NAME, ACTION_NAME)                                                                  \
    case eosio::hash_name(#ACTION_NAME):                                                                      \
        executed = eosio::execute_action(eosio::name(receiver), eosio::name(code), &CLASS_NAME::ACTION_NAME); \
        break;

namespace system_epn {

    namespace actions {
        using contract_c1 = paycontracts;
        using contract_c2 = donations;
        using fixedProps::contract_account;

        template <typename F>
        void for_each_action(F&& f)
        {
            f("addasset"_h, eosio::action_type_wrapper<&contract_c1::addasset>{}, ricardians::paycontract::addasset_ricardian, "type", "contract", "minAmount", "maxAmount");

            f("draftdon"_h, eosio::action_type_wrapper<&contract_c2::draftdon>{}, ricardians::donation::draftdon_ricardian, "owner", "contractID", "memoSuffix");
            f("signdon"_h, eosio::action_type_wrapper<&contract_c2::signdon>{}, ricardians::donation::signdon_ricardian, "signer", "drafter", "contractID", "quantity", "frequency", "signerMemo");
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


// Final part of the dispatcher
EOSIO_ACTION_DISPATCHER(system_epn::actions)

// Things to populate the ABI with
EOSIO_ABIGEN(actions(system_epn::actions),
             table("signers"_n, system_epn::SignerData),
             table("drafts"_n, system_epn::DrafterData),
             ricardian_clause("Paycontracts clause", ricardians::paycontract::ricardian_clause),
             ricardian_clause("Donations clause", ricardians::donation::ricardian_clause))
