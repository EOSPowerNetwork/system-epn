#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <string>

#include "paycontracts.hpp"

using namespace eosio;
using std::string;
using namespace system_epn;

paycontracts::paycontracts(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds)
{
    /* NOP */
}

void paycontracts::sayhi()
{
    print("Hi");
}

void paycontracts::sayhialice(const name& someone)
{
    check(someone == "alice"_n, "You may only say hi to Alice!");
    print("Hi, Alice!");
}
