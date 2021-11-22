#include "donations.hpp"

const char* system_epn::draftdon_ricardian = R"(
This action allows the drafter to create a donation contract, which signers may use to donate cryptocurrencies to the drafter. 
)";

const char* system_epn::signdon_ricardian = R"(
This action allows a signer to sign an existing donation contract. This will allow the EPN to automatically 
withdraw the funds of the signer according to the conditions specified in this action.
)";

const char* system_epn::ricardian_clause2 = R"(
This is donations ricardian clause.
)";
