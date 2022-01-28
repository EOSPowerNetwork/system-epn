#include "_dispatcher.hpp"
#include "donations.hpp"
#include "paycontracts.hpp"

// Final part of the dispatcher
EOSIO_ACTION_DISPATCHER(system_epn::actions)

// Things to populate the ABI with
EOSIO_ABIGEN(actions(system_epn::actions),
             table("signers"_n, system_epn::SignerData),
             table("drafts"_n, system_epn::DrafterData),
             ricardian_clause("Paycontracts clause", ricardians::paycontract::ricardian_clause),
             ricardian_clause("Donations clause", ricardians::donation::ricardian_clause))
