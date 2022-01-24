#include "epn_test_nodeos.hpp"

using namespace system_epn;

namespace system_epn::test_nodeos
{
    int32_t start(system_epn::epn_test_chain& t) {
        // Make all prior transactions irreversible. This causes the transactions to
        // go into the block log.
        t.getChain().finish_block();
        t.getChain().finish_block();

        // Copy blocks.log into a fresh directory for nodeos to use
        eosio::execute("rm -rf example_chain");
        eosio::execute("mkdir -p example_chain/blocks");
        eosio::execute("cp " + t.getChain().get_path() + "/blocks/blocks.log example_chain/blocks");

        // Run nodeos
        int32_t ret = 0;
        ret = eosio::execute(
            "nodeos -d example_chain "
            "--config-dir example_config "
            "--plugin eosio::chain_plugin "
            "--plugin eosio::chain_api_plugin "
            "--plugin eosio::history_plugin "
            "--plugin eosio::history_api_plugin "
            "--plugin eosio::producer_plugin "
            "--plugin eosio::http_plugin --verbose-http-errors "
            "--plugin eosio::epn_plugin --permission \"active\" --operator-name \"eosio\" --signing-private-key \"5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3\" "
            "--access-control-allow-origin \"*\" "
            "--access-control-allow-header \"*\" "
            "--http-validate-host 0 "
            "--http-server-address 0.0.0.0:8888 "
            "--contracts-console "
            "-e -p eosio");

        return ret;
    }

    int32_t get_info() {
        auto get_info = []() { return eosio::execute("cleos get info"); };
        for (uint64_t count = 0; count < 1e9 && get_info() != 0; ++count) {
            // NOP
        }
        return get_info();
    }

    int32_t stop() {
        return eosio::execute("kill `ps -A | grep [n]odeos | awk '{print $1}'`");
    }
}  // namespace system_epn::test_nodeos
