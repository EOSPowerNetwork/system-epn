#include "errormessages.hpp"
#include "include/helpers.hpp"

using namespace system_epn::actions;

constexpr auto testsuite_donations = "[Donations]";

// Verify that a donation is able to be drafted
TEST_CASE("Draft", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n), nullptr);
}

// Verify that the same owner can create two contracts with different contractIDs
TEST_CASE("Draft 2 unique", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "donation1"_n), nullptr);
    expect(alice.trace<draftdon>(owner, "donation2"_n), nullptr);
}

// Verify that the same owner cannot create two contracts with the same contractID
TEST_CASE("Draft 2 identical", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n), nullptr);
    expect(alice.trace<draftdon>(owner, "mydonation"_n), error::doubleDraft.data());
}

// Verify that the owner must be a signer on the draft
TEST_CASE("Draft wrong owner", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "bob"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n), error::wrongOwner.data());
}

// Verify that two different drafters can use the same contractID
TEST_CASE("Same contract ID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto [alice, bob] = get2Acc(t);

    // Contract IDs must only be unique under the same owner
    expect(alice.trace<draftdon>("alice"_n, "mydonation"_n), nullptr);
    expect(bob.trace<draftdon>("bob"_n, "mydonation"_n), nullptr);
}
