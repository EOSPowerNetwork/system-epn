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

void donations::draftdon(const name& owner, const name& contractID, const Memo& memoSuffix, const bool drafterPaysSignerRAM)
{
    require_auth(owner);

    DonationsTable _donations(get_self(), owner.value);
    _donations.draft(contractID, memoSuffix, drafterPaysSignerRAM);
}

void donations::signdon(const name& signer, const name& owner, const name& contractID, const asset& quantity, const uint32_t& frequency, const Memo& signerMemo)
{
    // Todo
}

//void donations::signdon(const name& to, const asset& amount, const uint32_t& frequency) {}