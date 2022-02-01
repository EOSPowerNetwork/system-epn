#include "donations.hpp"

#include <eosio/action.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <string>
#include <token/token.hpp>

#include "core/errormessages.hpp"
#include "core/fixedprops.hpp"

#include "interface/include/donationsIntf.hpp"

using namespace eosio;
using namespace system_epn;
using std::string;

void donations::draftdon(const name& owner, const name& contractID, const Memo& memoSuffix) {
    require_auth(owner);

    DonationsIntf::draft(owner, contractID, memoSuffix);
}

void donations::signdon(const name& signer, const name& drafter, const name& contractID, const Asset& quantity, const Frequency& frequency, const Memo& signerMemo) {
    require_auth(signer);
    auto contract = DonationsIntf(drafter, contractID);
    contract.sign(signer, quantity, frequency, signerMemo);

    auto signerPermission = permission_level(signer, "active"_n);  // Todo - change the permission level to whatever is specified by the signer
    transfer(signer, drafter, signerPermission, quantity, signerMemo, contract.getMemoSuffix());
}

void system_epn::transfer(const name& from, const name& to, const permission_level& permission, const Asset& payment, const Memo& signerMemo, const Memo& drafterMemo) {
    // Calculate memos
    string toOrgMemo = string(fixedProps::memo::marketingMemo) + signerMemo.value + string(fixedProps::memo::memoDelimiter) + drafterMemo.value;
    string feeMemo = string(fixedProps::memo::marketingMemo) + string(fixedProps::memo::feeMemo);

    // Calculate the two transfer amounts (donation and fee)
    asset feeAmt{static_cast<int64_t>(payment.value.amount * fixedProps::Assets::transactionFee), payment.value.symbol};
    asset drafterAmt{payment.value.amount - feeAmt.amount, payment.value.symbol};
    check(feeAmt.amount > 0, error::invalidPaymentAmount.data());
    check(drafterAmt.amount > 0, error::invalidPaymentAmount.data());

    // Execute the two token transfers
    token::actions::transfer _transfer{Asset::getTokenContract(payment), permission};
    _transfer.send(from, to, drafterAmt, toOrgMemo);
    _transfer.send(from, fixedProps::revenue_account, feeAmt, feeMemo);
}