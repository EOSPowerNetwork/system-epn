#include "_dispatcher.hpp"
#include "donations.hpp"
#include "paycontracts.hpp"

// Final part of the dispatcher
EOSIO_ACTION_DISPATCHER(system_epn::actions)

// Things to populate the ABI with
EOSIO_ABIGEN(actions(system_epn::actions),
             table("donations"_n, system_epn::Donation),
             ricardian_clause("Class 1 clause", system_epn::ricardian_clause),
             ricardian_clause("Class 2 clause", system_epn::ricardian_clause2))
