#pragma once

namespace ricardians {
    namespace paycontract {
        const char* addasset_ricardian = R"(
        This action adds a new asset as an official token supported by the EPN.
        )";

        const char* ricardian_clause = R"(
        )";
    }

    namespace donation {
        const char* draftdon_ricardian = R"(
        This action allows the drafter to create a donation contract, which signers may use to donate cryptocurrencies to the drafter. 
        )";

        const char* signdon_ricardian = R"(
        This action allows a signer to sign an existing donation contract. This will allow the EPN to automatically 
        withdraw the funds of the signer according to the conditions specified in this action.
        )";

        const char* ricardian_clause = R"(
        This is donations ricardian clause.
        )";
    }
}