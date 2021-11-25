#include <eosio/asset.hpp>

#include <iostream>
#include <vector>
#include "errormessages.hpp"
#include "fixedprops.hpp"
#include "include/helpers.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace system_epn::actions;
using system_epn::Frequency;
using system_epn::Memo;

using eosio::asset;
using std::string;
using std::string_view;
using std::vector;

///////////////////////////////////////////////////////////////////////////////////////
constexpr auto testsuite_donations = "[Donations]";
SCENARIO("0. Data type tests:", testsuite_donations)
{
    GIVEN("A valid memo")
    {
        constexpr string_view passing_memo = "VeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
        static_assert(passing_memo.size() == fixedProps::memo::memoSize);
        THEN("A Memo object can be constructed")
        {
            REQUIRE(Memo::validate(passing_memo) == true);
        }
    }

    GIVEN("An invalid memo")
    {
        constexpr string_view failing_memo = "AVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
        static_assert(failing_memo.size() == fixedProps::memo::memoSize + 1);
        THEN("A Memo object cannot be constructed")
        {
            REQUIRE(Memo::validate(failing_memo) == false);
        }
    }

    GIVEN("A valid frequency")
    {
        constexpr uint32_t validFrequency = fixedProps::Frequency::minimum_frequency_seconds;
        THEN("A Frequency object can be constructed")
        {
            REQUIRE(Frequency::validate(validFrequency) == true);
        }
    }

    GIVEN("An invalid frequency")
    {
        constexpr uint32_t invalidFrequency1 = fixedProps::Frequency::minimum_frequency_seconds - 1;
        constexpr uint32_t invalidFrequency2 = fixedProps::Frequency::maximum_frequency_seconds + 1;
        THEN("A Frequency object cannot be constructed")
        {
            REQUIRE(Frequency::validate(invalidFrequency1) == false);
            REQUIRE(Frequency::validate(invalidFrequency2) == false);
        }
    }
}

SCENARIO("1. A single drafter using the draftdon action", testsuite_donations)
{
    GIVEN("An initial empty chain setup")
    {
        test_chain t;
        setupChain(t);
        auto alice{getAcc(t)};
        auto code = system_epn::contract_account;
        auto owner = "alice"_n;
        auto contractID = "donation1"_n;

        THEN("Alice's donation should not exist")
        {
            system_epn::donations::DonationsTable state(code, owner.value);
            auto donationIter = state.find(contractID.value);
            REQUIRE(donationIter == state.end());
        }

        WHEN("Alice creates a donation")
        {
            Memo memo{"Memo"};
            bool drafterPaysSignerRAM = false;

            auto trace = alice.trace<draftdon>(owner, contractID, memo, drafterPaysSignerRAM);
            expect(trace, nullptr);
            printRamDeltas(trace);

            THEN("The donation contract should exist")
            {
                system_epn::donations::DonationsTable state(code, owner.value);
                auto donationIter = state.find(contractID.value);
                REQUIRE(donationIter != state.end());                                 // "Donation not saved to state"
                REQUIRE(donationIter->contractID == contractID);                      // "ContractID not saved properly"
                REQUIRE(donationIter->memoSuffix == memo);                            // "Memo not saved properly"
                REQUIRE(donationIter->drafterPaysSignerRAM == drafterPaysSignerRAM);  // "drafterPaysSignerRAM not saved properly"
                REQUIRE(std::empty(donationIter->signerData));                        // "Spurious signer data"
            }

            THEN("Alice is the only one whose RAM is consumed")
            {
                const vector<action_trace>& actions = trace.action_traces;
                check(actions.size() == 1, "More than one action? This should never happen.");
                const auto& ramDeltas = actions.at(0).account_ram_deltas;
                REQUIRE(ramDeltas.size() == 1);

                const auto& delta = ramDeltas.at(0);
                REQUIRE(delta.account == "alice"_n);

                AND_THEN("Alice should have consumed the expected amount of RAM")
                {
                    constexpr int64_t DraftDonRamConsumption = 239;
                    REQUIRE(delta.delta == DraftDonRamConsumption);
                }
            }

            THEN("A second donation may be created using a different name")
            {
                auto trace2 = alice.trace<draftdon>(owner, "donation2"_n, "Memo", false);
                CHECK(succeeded(trace2));
            }

            THEN("A second donation with the same name cannot be created")
            {
                t.start_block(1000);
                auto trace3 = alice.trace<draftdon>(owner, contractID, "Memo", false);
                CHECK(failedWith(trace3, error::doubleDraft));
            }
        }
    }
}

SCENARIO("2. Two drafters using the draftdon action", testsuite_donations)
{
    GIVEN("An initial empty chain setup")
    {
        test_chain t;
        setupChain(t);

        AND_GIVEN("Two different potential drafters, Alice and Bob")
        {
            auto [alice, bob] = get2Acc(t);

            THEN("Alice cannot create a donation for Bob without his authorization")
            {
                auto trace = alice.trace<draftdon>("bob"_n, "mydonation"_n, "Memo", false);
                REQUIRE(failedWith(trace, error::missingAuth));
            }

            WHEN("Alice creates a donation")
            {
                name aliceContractName = "adonation"_n;
                alice.act<draftdon>("alice"_n, aliceContractName, "Memo", false);

                THEN("Bob can also create a contract with the same name")
                {
                    auto trace = bob.trace<draftdon>("bob"_n, aliceContractName, "Memo", false);
                    REQUIRE(succeeded(trace));
                }
            }
        }
    }
}

// TEST_CASE("7. Verify that a donation can be signed & state is accurate", testsuite_donations)
// {
//     test_chain t;
//     setupChain(t);
//     auto [alice, bob] = get2Acc(t);

//     // Write transaction
//     auto owner = "alice"_n;
//     auto contractID = "mydonation"_n;
//     string drafterMemo = "Drafter memo";
//     expect(alice.trace<draftdon>(owner, contractID, drafterMemo, false), nullptr);

//     asset quantity = s2a("1.0000 EOS");
//     uint32_t frequency = 60 * 60 * 23;  // 23 hours
//     string signerMemo = "Signer memo";
//     expect(bob.trace<signdon>("bob"_n, owner, contractID, quantity, frequency, signerMemo), nullptr);

//     system_epn::donations::DonationsTable state(system_epn::contract_account, owner.value);
//     auto iter = state.require_find(contractID.value);
//     check(iter->signerData.size() == 1, "Signer data not saved");
//     auto signerData = iter->signerData.at(0);
//     check(signerData.frequency == frequency, "Donation frequency not stored correctly");
//     check(signerData.quantity == quantity, "Donation quantity not stored correctly");
//     check(signerData.signerMemo == signerMemo, "Signer memo not stored correctly");
// }

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
