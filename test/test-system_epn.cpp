#include <eosio/asset.hpp>

#include <iostream>
#include <vector>
#include "errormessages.hpp"
#include "fixedprops.hpp"
#include "include/helpers.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace system_epn::actions;
using namespace TestData;
using system_epn::Frequency;
using system_epn::Memo;

using eosio::asset;
using eosio::symbol_code;
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

            THEN("The donation contract should exist")
            {
                system_epn::donations::DonationsTable state(code, owner.value);
                auto donationIter = state.find(contractID.value);
                REQUIRE(donationIter != state.end());                                 // "Donation not saved to state"
                REQUIRE(donationIter->contractID == contractID);                      // "ContractID not saved properly"
                REQUIRE(donationIter->memoSuffix == memo);                            // "Memo not saved properly"
                REQUIRE(donationIter->drafterPaysSignerRAM == drafterPaysSignerRAM);  // "drafterPaysSignerRAM not saved properly"
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

SCENARIO("3. A single signer using the \"signdon\" action to sign a single donation draft", testsuite_donations)
{
    GIVEN("A chain in which Alice drafted a donation, with signer paying additonial RAM cost")
    {
        test_chain t;
        setupChain(t);
        setup_EOS_token(t);

        auto [alice, bob] = get2Acc(t);
        auto code = system_epn::contract_account;
        auto owner = "alice"_n;
        auto contractID = "donation1"_n;
        bool drafterPaysSignerRAM = false;
        asset initialFunds = s2a("1000.0000 EOS");
        Memo drafterMemo{"Memo"};
        Memo signerMemo{"Signer memo"};

        alice.act<draftdon>(owner, contractID, drafterMemo, drafterPaysSignerRAM);

        AND_GIVEN("Alice and Bob each have 1,000 EOS")
        {
            setup_fundUser(t, "alice"_n, initialFunds);
            setup_fundUser(t, "bob"_n, initialFunds);

            THEN("The donation should have zero signers")
            {
                system_epn::donations::DonationsTable state(code, owner.value);
                auto donationIter = state.find(contractID.value);
                REQUIRE(donationIter != state.end());           // "Donation not saved to state"
                REQUIRE(std::empty(donationIter->signerData));  // "Spurious signer data"
            }

            THEN("Alice cannot sign her own donation")
            {
                asset q{s2a("1.0000 EOS")};
                auto t = alice.trace<signdon>("alice"_n, owner, contractID, q, freq_23Hours, signerMemo);
                CHECK(failedWith(t, error::invalidSigner));
            }

            THEN("Bob cannot sign Alice's donation with an invalid currency")
            {
                // Todo - wrap asset with EPNAsset, and this test can be covered by a data type test
                asset q{s2a("1.0000 FAKE")};
                auto t = bob.trace<signdon>("bob"_n, owner, contractID, q, freq_23Hours, signerMemo);
                CHECK(failedWith(t, error::invalidCurrency));
            }

            THEN("Bob can sign Alice's donation")
            {
                asset q{s2a("1.0000 EOS")};
                name signer = "bob"_n;
                auto t = bob.trace<signdon>(signer, owner, contractID, q, freq_23Hours, signerMemo);
                CHECK(succeeded(t));
            }

            WHEN("Bob signs Alice's donation")
            {
                asset donationAmount{s2a("1.0000 EOS")};
                name signer = "bob"_n;
                auto trace = bob.trace<signdon>(signer, owner, contractID, donationAmount, freq_23Hours, signerMemo);

                THEN("The donation should have exactly one signer")
                {
                    system_epn::donations::DonationsTable state(code, owner.value);
                    auto donationIter = state.find(contractID.value);
                    REQUIRE(donationIter != state.end());           // "Donation not saved to state"
                    REQUIRE(donationIter->signerData.size() == 1);  // "Spurious signer data"
                }

                THEN("The signer should be charged the additional RAM cost")
                {
                    const vector<action_trace>& actions = trace.action_traces;
                    check(actions.size() == 1, "More than one action? This should never happen.");
                    const auto& ramDeltas = actions.at(0).account_ram_deltas;
                    REQUIRE(ramDeltas.size() == 1);

                    const auto& delta = ramDeltas.at(0);
                    REQUIRE(delta.account == signer);
                }

                THEN("Only the expected amount of RAM is consumed")
                {
                    const vector<action_trace>& actions = trace.action_traces;
                    const auto& ramDeltas = actions.at(0).account_ram_deltas;
                    const auto& delta = ramDeltas.at(0);
                    constexpr int64_t DraftDonRamConsumption = 239;
                    REQUIRE(delta.delta == DraftDonRamConsumption);
                }

                THEN("Bob cannot sign Alice's donation again")
                {
                    auto trace2 = bob.trace<signdon>(signer, owner, contractID, donationAmount, freq_23Hours, signerMemo);
                    CHECK(failedWith(trace2, error::duplicateSigner));
                }

                THEN("Bob should have [donationAmount] less EOS")
                {
                    auto amount = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
                    CHECK(initialFunds - amount == donationAmount);
                }

                THEN("Alice should have [donationAmount] more EOS")
                {
                    auto amount = token::contract::get_balance("eosio.token"_n, "alice"_n, symbol_code({"EOS"}));
                    CHECK(amount - initialFunds == donationAmount);
                }

                WHEN("Less time has passed than the frequency")
                {
                    uint32_t frequencyInBlocks = freq_23Hours.value * 2;  // Seconds to blocks, 500ms block time
                    uint32_t notEnoughBlocks = frequencyInBlocks - 1;
                    t.start_block(notEnoughBlocks);
                    THEN("Bob has still only made one donation")
                    {
                        auto amount = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
                        CHECK(initialFunds - amount == donationAmount);
                    }
                }

                WHEN("Enough time has passed for one additional donation")
                {
                    uint32_t frequencyInBlocks = freq_23Hours.value * 2;  // Seconds to blocks, 500ms block time
                    t.start_block(frequencyInBlocks);
                    THEN("Bob has made one more donation")
                    {
                        auto totalDonated = 2 * donationAmount;
                        auto amount = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
                        CHECK(initialFunds - amount == totalDonated);
                    }
                }
            }
        }
    }
}
