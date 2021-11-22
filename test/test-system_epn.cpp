#include <eosio/asset.hpp>

#include "errormessages.hpp"
#include "fixedprops.hpp"
#include "include/helpers.hpp"

using namespace system_epn::actions;
using eosio::asset;
using std::string;
using std::string_view;

constexpr auto testsuite_donations = "[Donations]";
TEST_CASE("1. Verify that a donation is able to be drafted & state is accurate", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    // Write transaction
    auto owner = "alice"_n;
    auto contractID = "mydonation"_n;
    string memo = "Memo";
    bool drafterPaysSignerRAM = false;

    expect(alice.trace<draftdon>(owner, contractID, memo, drafterPaysSignerRAM), nullptr);

    system_epn::donations::DonationsTable state(system_epn::contract_account, owner.value);
    auto donationIter = state.find(contractID.value);
    check(donationIter != state.end(), "Donation not saved to state");
    check(donationIter->contractID == contractID, "ContractID not saved properly");
    check(donationIter->memoSuffix == memo, "Memo not saved properly");
    check(donationIter->drafterPaysSignerRAM == drafterPaysSignerRAM, "drafterPaysSignerRAM not saved properly");
    check(donationIter->signerData.size() == 0, "Spurious signer data");
}

TEST_CASE("2. Verify that the same owner can create two contracts with different contractIDs", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "donation1"_n, "Memo", false), nullptr);
    expect(alice.trace<draftdon>(owner, "donation2"_n, "Memo", false), nullptr);
}

TEST_CASE("3. Verify that the same owner cannot create two contracts with the same contractID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo", false), nullptr);
    t.start_block(1000);
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo", false), error::doubleDraft.data());
}

TEST_CASE("4. Verify that the owner must be a signer on the draft", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "bob"_n;
    expect(alice.trace<draftdon>(owner, "mydonation"_n, "Memo", false), error::wrongOwner.data());
}

TEST_CASE("5. Verify that two different drafters can use the same contractID", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto [alice, bob] = get2Acc(t);

    // Contract IDs must only be unique under the same owner
    expect(alice.trace<draftdon>("alice"_n, "mydonation"_n, "Memo", false), nullptr);
    expect(bob.trace<draftdon>("bob"_n, "mydonation"_n, "Memo", false), nullptr);
}

TEST_CASE("6. Verify that the drafter memo cannot exceed fixedProps::memoSize", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    auto owner = "alice"_n;
    constexpr string_view passing_memo = "VeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
    static_assert(passing_memo.size() == fixedProps::memo::memoSize);

    constexpr string_view failing_memo = "AVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
    static_assert(failing_memo.size() == fixedProps::memo::memoSize + 1);

    expect(alice.trace<draftdon>(owner, "mydonation"_n, passing_memo, false), nullptr);
    expect(alice.trace<draftdon>(owner, "mydonation2"_n, failing_memo, false), error::memoTooLong.data());
}

TEST_CASE("7. Verify that a donation can be signed & state is accurate", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto [alice, bob] = get2Acc(t);

    // Write transaction
    auto owner = "alice"_n;
    auto contractID = "mydonation"_n;
    string drafterMemo = "Drafter memo";
    expect(alice.trace<draftdon>(owner, contractID, drafterMemo, false), nullptr);

    asset quantity = s2a("1.0000 EOS");
    uint32_t frequency = 60 * 60 * 23;  // 23 hours
    string signerMemo = "Signer memo";
    expect(bob.trace<signdon>("bob"_n, owner, contractID, quantity, frequency, signerMemo), nullptr);

    system_epn::donations::DonationsTable state(system_epn::contract_account, owner.value);
    auto iter = state.require_find(contractID.value);
    check(iter->signerData.size() == 1, "Signer data not saved");
    auto signerData = iter->signerData.at(0);
    check(signerData.frequency == frequency, "Donation frequency not stored correctly");
    check(signerData.quantity == quantity, "Donation quantity not stored correctly");
    check(signerData.signerMemo == signerMemo, "Signer memo not stored correctly");
}

TEST_CASE("8. Verify that the RAM payer matches the payer specified by the drafter", testsuite_donations)
{
    // Todo
}

TEST_CASE("9. Verify that the signer has authorized the sign action", testsuite_donations)
{
    // Todo
}

TEST_CASE("10. Verify that the donation being signed actually exists", testsuite_donations)
{
    // Todo
}

TEST_CASE("11. Verify that the signer cannot sign the same donation twice", testsuite_donations)
{
    // Todo
}

TEST_CASE("12. Verify that drafter cannot sign their own donation", testsuite_donations)
{
    // Todo
}

TEST_CASE("13. Verify that multiple unique signers may sign a donation", testsuite_donations)
{
    // Todo
}

TEST_CASE("14. Verify that only the accepted token symbols can be used to donate", testsuite_donations)
{
    // Todo
}

TEST_CASE("15. Verify that only frequencies in the accepted range (min/max) can be used", testsuite_donations)
{
    // Todo
}

TEST_CASE("16. Veriify that the signer memo cannot exceed fixedProps::memoSize", testsuite_donations)
{
    // Todo
}

TEST_CASE("17. Verify that the first donation goes out immediately", testsuite_donations)
{
    // Todo
}

TEST_CASE("18. Verify that the second donation goes out correctly", testsuite_donations)
{
    // Todo
}
