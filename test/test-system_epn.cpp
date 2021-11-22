#include "errormessages.hpp"
#include "include/helpers.hpp"

using namespace system_epn::actions;

constexpr auto testsuite_donations = "[Donations]";

TEST_CASE("1. Verify that a donation is able to be drafted", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n), nullptr);
}

TEST_CASE("2. Verify that the same owner can create two contracts with different contractIDs", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "donation1"_n), nullptr);
    expect(alice.trace<draftdon>(owner, "donation2"_n), nullptr);
}

TEST_CASE("3. Verify that the same owner cannot create two contracts with the same contractID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n), nullptr);
    t.start_block(1000);
    expect(alice.trace<draftdon>(owner, "mydonation"_n), error::doubleDraft.data());
}

TEST_CASE("4. Verify that the owner must be a signer on the draft", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "bob"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n), error::wrongOwner.data());
}

TEST_CASE("5. Verify that two different drafters can use the same contractID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto [alice, bob] = get2Acc(t);

    // Contract IDs must only be unique under the same owner
    expect(alice.trace<draftdon>("alice"_n, "mydonation"_n), nullptr);
    expect(bob.trace<draftdon>("bob"_n, "mydonation"_n), nullptr);
}
