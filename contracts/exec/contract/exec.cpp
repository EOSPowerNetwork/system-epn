#include <string>

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>

#include "exec.hpp"

namespace system_epn
{
    using namespace eosio;
    using eosio::check;
    using eosio::contract;
    using eosio::datastream;
    using eosio::name;
    using eosio::print;
    using eosio::symbol;
    using std::string;

    exec::exec(name receiver, name code, datastream<const char*> ds)
        : contract(receiver, code, ds) {
        /* NOP */
    }

    void exec::execute() {
        /* NOP */
        print("execute");
    }

}  // namespace system_epn
