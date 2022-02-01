#include <eosio/asset.hpp>
#include <eosio/check.hpp>
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>
#include <eosio/tester.hpp>

#include <iostream>
#include <vector>

#include "core/errormessages.hpp"
#include "core/fixedprops.hpp"

#include "system/include/donations.hpp"
#include "system/interface/include/Donations.hpp"

#include "epn_test_chain.hpp"
#include "helpers.hpp"
#include "ramconsumption.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace system_epn::actions;
using namespace system_epn;
using system_epn::Asset;
using system_epn::DrafterMIType;
using system_epn::Frequency;
using system_epn::Memo;
using system_epn::SignerMIType;

using eosio::asset;
using eosio::check;
using eosio::milliseconds;
using eosio::symbol;
using eosio::symbol_code;

using std::find_if;
using std::string;
using std::string_view;
using std::to_string;
using std::vector;

using system_epn::checks::failedWith;
using system_epn::checks::succeeded;

const vector<name> regularUsers = {"alice"_n, "bob"_n, "charlie"_n, "dan"_n};
const vector<name> powerUsers = {"ethan"_n, "frank"_n, "gary"_n, "harry"_n};

///////////////////////////////////////////////////////////////////////////////////////
constexpr auto testsuite_donations = "[Donations]";
constexpr auto testsuite_nodeos = "[Nodeos]";
SCENARIO("0. Data type tests:", testsuite_donations) {
    GIVEN("A valid memo") {
        constexpr string_view passing_memo = "VeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
        static_assert(passing_memo.size() == fixedProps::memo::memoSize);
        THEN("A Memo object can be constructed") {
            CHECK(Memo::validate(passing_memo) == true);
        }
    }

    GIVEN("An invalid memo") {
        constexpr string_view failing_memo = "AVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongMemo";
        static_assert(failing_memo.size() == fixedProps::memo::memoSize + 1);
        THEN("A Memo object cannot be constructed") {
            CHECK(Memo::validate(failing_memo) == false);
        }
    }

    GIVEN("A valid frequency") {
        constexpr uint32_t validFrequency = fixedProps::Frequency::minimum_frequency_seconds;
        THEN("A Frequency object can be constructed") {
            CHECK(Frequency::validate(validFrequency) == true);
        }
    }

    GIVEN("An invalid frequency") {
        constexpr uint32_t invalidFrequency1 = fixedProps::Frequency::minimum_frequency_seconds - 1;
        constexpr uint32_t invalidFrequency2 = fixedProps::Frequency::maximum_frequency_seconds + 1;
        THEN("A Frequency object cannot be constructed") {
            CHECK(Frequency::validate(invalidFrequency1) == false);
            CHECK(Frequency::validate(invalidFrequency2) == false);
        }
    }

    GIVEN("An invalid test asset type") {
        asset invalidAsset(s2a("100.0000 HAX"));
        check(!fixedProps::Assets::getAssetProps(invalidAsset.symbol.code()), "Asset type must not be supported");

        THEN("An Asset object cannot be constructed") {
            CHECK(Asset::validate(invalidAsset) == false);
        }
    }

    GIVEN("A supported asset type with an out of range (over maximum) quantity") {
        eosio::symbol validSymbol("EOS", 4);
        auto props = fixedProps::Assets::getAssetProps(validSymbol.code());
        check(static_cast<bool>(props), "Asset type must be supported");

        auto max = props.value().maximum;
        int64_t invalidAmount = max + 1;
        check(invalidAmount > max, "Amount must be greater than maximum");

        asset invalidAsset(invalidAmount, validSymbol);

        THEN("An Asset object cannot be constructed") {
            CHECK(Asset::validate(invalidAsset) == false);
        }
    }

    GIVEN("A supported asset type with an out of range (under minimum) quantity") {
        eosio::symbol validSymbol("EOS", 4);
        auto props = fixedProps::Assets::getAssetProps(validSymbol.code());
        check(static_cast<bool>(props), "Asset type must be supported");

        auto min = props.value().minimum;
        int64_t invalidAmount = min - 1;
        check(invalidAmount < min, "Amount must be less than minimum");

        asset invalidAsset(invalidAmount, validSymbol);

        THEN("An Asset object cannot be constructed") {
            CHECK(Asset::validate(invalidAsset) == false);
        }
    }

    GIVEN("A supported asset type with a quantity in range") {
        eosio::symbol validSymbol("EOS", 4);
        auto props = fixedProps::Assets::getAssetProps(validSymbol.code());
        check(static_cast<bool>(props), "Asset type must be supported");

        auto min = props.value().minimum;
        auto max = props.value().maximum;
        auto validAmount1 = min;
        auto validAmount2 = max;

        check(validAmount1 >= min && validAmount1 <= max, "Amount must be within valid range");
        check(validAmount2 >= min && validAmount2 <= max, "Amount must be within valid range");

        asset validAsset1(validAmount1, validSymbol);
        asset validAsset2(validAmount2, validSymbol);

        THEN("An Asset object may be constructed") {
            CHECK(Asset::validate(validAsset1) == true);
            CHECK(Asset::validate(validAsset2) == true);
        }
    }
}

SCENARIO("1. A single drafter using the draftdon action", testsuite_donations) {
    GIVEN("An initial empty chain setup") {
        epn_test_chain t(regularUsers, powerUsers);
        auto alice = t.as("alice"_n);
        auto code = fixedProps::contract_account;
        auto owner = "alice"_n;
        auto contractID = "donation1"_n;

        THEN("Alice's donation should not exist") {
            DrafterMIType _drafts(code, owner.value);
            auto donationIter = _drafts.find(contractID.value);
            CHECK(donationIter == _drafts.end());
        }

        WHEN("Alice creates a donation") {
            Memo memo{"Memo"};

            auto trace = alice.trace<draftdon>(owner, contractID, memo);
            expect(trace, nullptr);

            THEN("The donation contract should exist") {
                DrafterMIType _drafts(code, owner.value);
                auto donationIter = _drafts.find(contractID.value);
                CHECK(donationIter != _drafts.end());           // "Donation not saved to state"
                CHECK(donationIter->contractID == contractID);  // "ContractID not saved properly"
                CHECK(donationIter->memoSuffix == memo);        // "Memo not saved properly"
            }

            THEN("Alice is the only one whose RAM is consumed") {
                const vector<action_trace>& actions = trace.action_traces;
                check(actions.size() == 1, "More than one action? This should never happen.");
                const auto& ramDeltas = actions.at(0).account_ram_deltas;
                CHECK(ramDeltas.size() == 1);

                const auto& delta = ramDeltas.at(0);
                CHECK(delta.account == "alice"_n);

                AND_THEN("Alice should have consumed the expected amount of RAM for first emplace") {
                    CHECK(delta.delta == ramConsumption_bytes::firstEmplace::DraftDonation);
                }
            }

            THEN("A second donation may be created using a different name") {
                auto trace2 = alice.trace<draftdon>(owner, "donation2"_n, "Memo");
                CHECK(succeeded(trace2));

                AND_THEN("Alice should have consumed the expected amount of RAM for subsequent emplace") {
                    const auto ramdelta = trace2.action_traces[0].account_ram_deltas[0];
                    CHECK(ramdelta.delta == ramConsumption_bytes::subsequentEmplace::DraftDonation);
                }
            }

            THEN("A second donation with the same name cannot be created") {
                t.getChain().start_block(1000);
                auto trace3 = alice.trace<draftdon>(owner, contractID, "Memo");
                CHECK(failedWith(trace3, error::doubleDraft));
            }
        }
    }
}

SCENARIO("2. Two drafters using the draftdon action", testsuite_donations) {
    GIVEN("An initial empty chain setup") {
        epn_test_chain t(regularUsers, powerUsers);

        AND_GIVEN("Two different potential drafters, Alice and Bob") {
            auto [alice, bob] = t.as("alice"_n, "bob"_n);

            THEN("Alice cannot create a donation for Bob without his authorization") {
                auto trace = alice.trace<draftdon>("bob"_n, "mydonation"_n, "Memo");
                CHECK(failedWith(trace, error::missingAuth));
            }

            WHEN("Alice creates a donation") {
                name aliceContractName = "adonation"_n;
                alice.act<draftdon>("alice"_n, aliceContractName, "Memo");

                THEN("Bob can also create a contract with the same name") {
                    auto trace = bob.trace<draftdon>("bob"_n, aliceContractName, "Memo");
                    CHECK(succeeded(trace));
                }
            }
        }
    }
}

SCENARIO("3. A single signer using the \"signdon\" action to sign a single donation draft", testsuite_donations) {
    GIVEN("Given a chain with no power users in which Alice drafted a donation") {
        epn_test_chain t(regularUsers, powerUsers);

        auto [alice, bob, charlie, ethan] = t.as("alice"_n, "bob"_n, "charlie"_n, "ethan"_n);
        auto code = fixedProps::contract_account;
        auto owner = "alice"_n;
        auto contractID = "donation1"_n;
        asset initialFunds = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
        Memo drafterMemo{"Memo"};
        Memo signerMemo{"Signer memo"};

        alice.act<draftdon>(owner, contractID, drafterMemo);

        THEN("The donation should have zero signers") {
            CHECK(DonationsIntf::getNumSigners(owner, contractID) == 0);
        }

        THEN("Alice cannot sign her own donation") {
            asset q{s2a("1.0000 EOS")};
            auto t = alice.trace<signdon>("alice"_n, owner, contractID, q, fixedProps::Frequency::minimum_frequency_seconds, signerMemo);
            CHECK(failedWith(t, error::invalidSigner));
        }

        THEN("Bob cannot sign Alice's donation") {
            asset q{s2a("1.0000 EOS")};
            name signer = "bob"_n;
            name drafter = owner;
            auto t = bob.trace<signdon>(signer, drafter, contractID, q, fixedProps::Frequency::minimum_frequency_seconds, signerMemo);
            CHECK(failedWith(t, error::missingPermission.data()));
        }

        WHEN("Bob registers as a power user") {
            t.setup_setPowerUserAccount(bob);

            THEN("Bob can sign Alice's donation") {
                asset q{s2a("1.0000 EOS")};
                name signer = "bob"_n;
                name drafter = owner;
                auto t = bob.trace<signdon>(signer, drafter, contractID, q, fixedProps::Frequency::minimum_frequency_seconds, signerMemo);
                expect(t, nullptr);
                CHECK(succeeded(t));
            }

            WHEN("Bob signs Alice's donation") {
                asset donationAmount{s2a("1.0000 EOS")};
                name signer = "bob"_n;
                auto trace = bob.trace<signdon>(signer, owner, contractID, donationAmount, fixedProps::Frequency::minimum_frequency_seconds, signerMemo);
                expect(trace, nullptr);

                THEN("The donation should have exactly one signer") {
                    CHECK(DonationsIntf::getNumSigners(owner, contractID) == 1);
                }

                THEN("The signer consumes the expected amount of RAM") {
                    auto ramDeltas = debug::getFirstRamDeltaSummary(trace);
                    CHECK(ramDeltas.size() == 1);            // Only one account should have a ram delta
                    CHECK(ramDeltas[0].account == "bob"_n);  // And it should be the signer, Bob
                    CHECK(debug::getRamDelta(ramDeltas[0], "bob"_n) == ramConsumption_bytes::firstEmplace::SignDonation);
                }

                THEN("Bob cannot sign Alice's donation again") {
                    Memo anotherSignerMemo{"Let's subscribe again!"};
                    auto trace2 = bob.trace<signdon>(signer, owner, contractID, donationAmount, fixedProps::Frequency::minimum_frequency_seconds, anotherSignerMemo);
                    CHECK(failedWith(trace2, error::duplicateSigner));
                }

                THEN("The scheduled block to service the transaction should be correct") {
                    /* Note: test_chain head block timestamp shows the timestamp of the last completed block,
                    current_block() in the contract shows the timestamp of the current (incomplete) block. */

                    // Calculate expected service block timestamp
                    int64_t blockTime = milliseconds(500).count();  // in microseconds
                    int64_t lastProducedBlockTimestamp = t.getChain().get_head_block_info().timestamp.to_time_point().elapsed.count();
                    int64_t activeBlockTimestamp = lastProducedBlockTimestamp + blockTime;
                    int64_t serviceBlockTimestamp = activeBlockTimestamp + seconds(fixedProps::Frequency::minimum_frequency_seconds).count();

                    // Get the actual service block timestamp from state
                    auto row = DonationsIntf::getSignature(owner, contractID, signer);
                    int64_t contractServiceBlockTimestamp = row.serviceBlock.to_time_point().elapsed.count();

                    // Confirm they match
                    CHECK(serviceBlockTimestamp == contractServiceBlockTimestamp);
                }

                THEN("Bob should have [donationAmount] less EOS") {
                    auto amount = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
                    CHECK(initialFunds - amount == donationAmount);
                }

                THEN("Alice should have [donationAmount - fee] more EOS") {
                    // Calculate what balance Alice should have based on the donation amount
                    asset feeAmount(static_cast<int64_t>(donationAmount.amount * fixedProps::Assets::transactionFee), symbol("EOS", 4));
                    check(donationAmount > feeAmount, "Must be true");
                    asset calculatedBalance = initialFunds + (donationAmount - feeAmount);

                    // Get Alice's actual balance
                    auto realBalance = token::contract::get_balance("eosio.token"_n, "alice"_n, symbol_code({"EOS"}));

                    // Confirm they are equal
                    CHECK(realBalance == calculatedBalance);
                }

                THEN("EPN revenue account should have [fee] more EOS") {
                    asset feeAmount(static_cast<int64_t>(donationAmount.amount * fixedProps::Assets::transactionFee), symbol("EOS", 4));
                    auto realBalance = token::contract::get_balance("eosio.token"_n, fixedProps::revenue_account, symbol_code({"EOS"}));
                    CHECK(realBalance == feeAmount);
                }
            }
        }
    }
}

SCENARIO("4. A draft is signed in the context of a chain running the EPN plugin", testsuite_nodeos) {
    GIVEN("Nodeos with the EPN plugin is running") {
        epn_test_chain t(regularUsers, powerUsers);
        auto [alice, ethan] = t.as("alice"_n, "ethan"_n);  // Alice is regular user, ethan is power user

        // Drafter drafts donation
        auto owner = "alice"_n;
        auto contractID = "donation1"_n;
        auto t1 = alice.trace<draftdon>(owner, contractID, "Memo");
        expect(t1, nullptr);

        // Signer signs donation
        name signer = "ethan"_n;
        auto donationAmount = s2a("1.0000 EOS");
        auto t2 = ethan.trace<signdon>(signer, owner, contractID, donationAmount, fixedProps::Frequency::minimum_frequency_seconds, "Signer Memo");
        expect(t2, nullptr);

        auto aliceAmount = token::contract::get_balance("eosio.token"_n, "alice"_n, symbol_code({"EOS"}));
        auto ethanAmount = token::contract::get_balance("eosio.token"_n, "ethan"_n, symbol_code({"EOS"}));
        auto aliceAdded = donationAmount - eosio::asset(static_cast<int64_t>(donationAmount.amount * fixedProps::Assets::transactionFee), donationAmount.symbol);
        CHECK(aliceAmount == constants::user_balance + aliceAdded);
        CHECK(ethanAmount == constants::user_balance - donationAmount);

        t.launch_nodeos();
    }

    // WHEN("Less time has passed than the frequency") {
    //     uint32_t frequencyInBlocks = fixedProps::Frequency::minimum_frequency_seconds.value * 2;  // Seconds to blocks, 500ms block time
    //     uint32_t notEnoughBlocks = frequencyInBlocks - 1;
    //     t.start_block(notEnoughBlocks);
    //     THEN("Bob has still only made one donation") {
    //         auto amount = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
    //         CHECK(initialFunds - amount == donationAmount);
    //     }
    // }

    // WHEN("Enough time has passed for one additional donation") {
    //     uint32_t frequencyInBlocks = fixedProps::Frequency::minimum_frequency_seconds.value * 2;  // Seconds to blocks, 500ms block time
    //     t.start_block(frequencyInBlocks);
    //     THEN("Bob has made one more donation") {
    //         auto totalDonated = 2 * donationAmount;
    //         auto amount = token::contract::get_balance("eosio.token"_n, "bob"_n, symbol_code({"EOS"}));
    //         CHECK(initialFunds - amount == totalDonated);
    //     }
    // }
}

// Also, I think I should write an EOS Power Network security guide, to include people in my security-related design decisions.
