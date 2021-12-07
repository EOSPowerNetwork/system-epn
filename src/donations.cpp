#include <eosio/action.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>

#include <string>

#include "donations.hpp"
#include "errormessages.hpp"
#include "fixedprops.hpp"

using namespace eosio;
using namespace system_epn;
using eosio::time_point_sec;
using std::string;

void donations::draftdon(const name& owner, const name& contractID, const Memo& memoSuffix)
{
    require_auth(owner);

    print("Current time point in microseconds: " + eosio::microseconds_to_str(current_time_point().elapsed.count()));
    print("Current block time: ");
    //print(current_block_time());

    Donations().draft(owner, contractID, memoSuffix);
}

void donations::signdon(const name& signer, const name& drafter, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo)
{
    require_auth(signer);
    Donations().getDonation(drafter, contractID).sign(signer, quantity, frequency, signerMemo);
}
