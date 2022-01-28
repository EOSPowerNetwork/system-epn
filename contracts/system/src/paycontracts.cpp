#include <string>

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>

#include "paycontracts.hpp"

using namespace eosio;
using std::string;
using namespace system_epn;

paycontracts::paycontracts(name receiver, name code, datastream<const char*> ds)
    : contract(receiver, code, ds)
{
    /* NOP */
}

void paycontracts::addasset(const symbol& type, const name& contract, const int64_t& minAmount, const int64_t& maxAmount)
{
    /* NOP */
}
