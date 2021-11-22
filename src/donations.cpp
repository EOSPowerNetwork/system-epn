#include <eosio/action.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>

#include "donations.hpp"
#include "errormessages.hpp"
#include "fixedprops.hpp"

using namespace eosio;
using std::string;
using namespace system_epn;

donations::donations(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds)
{
    /* NOP */
}

void donations::draftdon(const name& owner, const name& contractID, const std::string& memoSuffix, const bool drafterPaysSignerRAM)
{
    require_auth(owner);

    check(memoSuffix.size() <= fixedProps::memo::memoSize, error::memoTooLong.data());

    DonationsTable _donations(get_self(), owner.value);
    check(_donations.find(contractID.value) == _donations.end(), error::doubleDraft.data());

    _donations.emplace(owner, [&](auto& row) {
        row.contractID = contractID;
        row.memoSuffix = memoSuffix;
        row.drafterPaysSignerRAM = drafterPaysSignerRAM;
    });
}

void donations::signdon(const name& signer, const name& owner, const name& contractID, const asset& quantity, const uint32_t& frequency, const string& signerMemo)
{
    // Todo
    // require_auth(signer);
}

//void donations::signdon(const name& to, const asset& amount, const uint32_t& frequency) {}