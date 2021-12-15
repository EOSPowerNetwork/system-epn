#include <eosio/action.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <token/token.hpp>

#include <string>

#include "donations.hpp"
#include "errormessages.hpp"
#include "fixedprops.hpp"

using namespace eosio;
using namespace system_epn;
using std::string;

void donations::draftdon(const name& owner, const name& contractID, const Memo& memoSuffix) {
    require_auth(owner);

    Donations().draft(owner, contractID, memoSuffix);
}

void donations::signdon(const name& signer, const name& drafter, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo) {
    require_auth(signer);
    Donations().getDonation(drafter, contractID).sign(signer, quantity, frequency, signerMemo);

    transfer(signer, drafter, permission_level(signer, "active"_n), quantity.value, signerMemo.value);
}

void system_epn::transfer(const name& from, const name& to, const permission_level& permission, const Asset& payment, const Memo& memo) {
    token::actions::transfer _transfer{Asset::getTokenContract(payment), permission};
    _transfer.send(from, to, payment.value, memo.value);
}
