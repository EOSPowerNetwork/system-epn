#include "errormessages.hpp"
#include "include/helpers.hpp"

using namespace system_epn::actions;

void setupChain(test_chain& t)
{
    setup_installMyContract(t);
    setup_createAccounts(t);
}
constexpr auto testsuite_donations = "[Donations]";

/* Happy test cases for drafting a new donation:
* Draft a donation
* Draft two separate donations
*/

TEST_CASE("Draft", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    expect(alice.trace<draftdon>("mydonation"_n), nullptr);
}

TEST_CASE("Draft 2 unique", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    expect(alice.trace<draftdon>("donation1"_n), nullptr);
    expect(alice.trace<draftdon>("donation2"_n), nullptr);
}

TEST_CASE("Draft 2 identical", testsuite_donations)
{
    test_chain t;
    setupChain(t);
    auto alice = getAcc(t);

    expect(alice.trace<draftdon>("mydonation"_n), nullptr);
    expect(alice.trace<draftdon>("mydonation"_n), error::donations::doubledraft.data());
}
