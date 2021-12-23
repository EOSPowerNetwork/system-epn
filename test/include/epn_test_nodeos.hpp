#pragma once
#include "epn_test_chain.hpp"

namespace system_epn
{
    namespace test_nodeos
    {
        int32_t start(system_epn::epn_test_chain& t);
        int32_t stop();
        int32_t get_info();
    };  // namespace test_nodeos

}  // namespace system_epn