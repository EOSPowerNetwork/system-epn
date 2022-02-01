# Donations tables
Multi-index tables related to the drafting and signing of Donation contracts.
_Todo: - Review these schemas, this document is outdated_

## Interface: Struct Donation
* Scope: Owner (Drafter of the donation pays the RAM cost for the donations table)

| Name of column | Data type | Description |
| ----------- | ----------- | ----------- |
| contractID | eosio::name | Unique ID of this donation contract in this scope |
| memoSuffix | eosio::name | Portion of the memo specified by the drafter used when this payment contract is executed |

### Interface: Struct Signature
| Name of column | Data type | Description |
| ----------- | ----------- | ----------- |
| signer | eosio::name | The account name of the signer |
| quantity | eosio::asset | The amount and type of token used in this donation contract |
| frequency | uint32_t | The number of seconds between each time this contract should be serviced |
| signerMemo | std::string | Portion of the memo specified by the signer used when this payment contract is executed |


### Example

| contractID  | memoSuffix | signerData |
| -----------  | ----------- | ----------- |
| myDonation  | "Thanks, from TheCompany!"  | { [ {"Bob"_n, "1.0000 EOS", 82800, "Donation to TheCompany"}, ... ] } |

