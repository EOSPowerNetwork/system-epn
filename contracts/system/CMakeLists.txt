set(CONTRACT_NAME "system")
set(CONTRACT_FILES "src/paycontracts.cpp" "src/donations.cpp")
set(INTERFACE_FILES "interface/donationsIntf.cpp")
set(TEST_FILES "test/helpers.cpp" "test/epn_test_chain.cpp" "test/test-system.cpp")

# Builds:
#  - Two contract wasms (release and debug)
#  - One test wasm
#  - One abi
add_contract(${CONTRACT_NAME})

