#include "errormessages.hpp"
#include "fixedprops.hpp"
#include "include/helpers.hpp"

using namespace system_epn::actions;

constexpr auto testsuite_donations = "[Donations]";

TEST_CASE("1. Verify that a donation is able to be drafted", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo"), nullptr);
}

TEST_CASE("2. Verify that the same owner can create two contracts with different contractIDs", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "donation1"_n, "Memo"), nullptr);
    expect(alice.trace<draftdon>(owner, "donation2"_n, "Memo"), nullptr);
}

TEST_CASE("3. Verify that the same owner cannot create two contracts with the same contractID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo"), nullptr);
    t.start_block(1000);
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo"), error::doubleDraft.data());
}

TEST_CASE("4. Verify that the owner must be a signer on the draft", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "bob"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo"), error::wrongOwner.data());
}

TEST_CASE("5. Verify that two different drafters can use the same contractID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto [alice, bob] = get2Acc(t);

    // Contract IDs must only be unique under the same owner
    expect(alice.trace<draftdon>("alice"_n, "mydonation"_n, "Memo"), nullptr);
    expect(bob.trace<draftdon>("bob"_n, "mydonation"_n, "Memo"), nullptr);
}

TEST_CASE("6. Verify that the memo suffix cannot exceed fixedProps::memoSize", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    constexpr std::string_view passing_memo = "VeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
    static_assert(passing_memo.size() == fixedProps::memo::memoSize);

    constexpr std::string_view failing_memo = "AVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
    static_assert(failing_memo.size() == fixedProps::memo::memoSize + 1);

    expect(alice.trace<draftdon>(owner, "mydonation"_n, passing_memo), nullptr);
    expect(alice.trace<draftdon>(owner, "mydonation2"_n, failing_memo), error::memoTooLong.data());
}

// Todo - Add memo length tests