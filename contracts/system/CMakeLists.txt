set(CONTRACT_NAME "system")
set(CONTRACT_FILES "contract/paycontracts.cpp" "contract/donations.cpp")
set(INTERFACE_FILES "interface/donationsIntf.cpp")
set(TEST_FILES "test/helpers.cpp" "test/epn_test_chain.cpp" "test/test-system.cpp")

add_contract(${CONTRACT_NAME})

