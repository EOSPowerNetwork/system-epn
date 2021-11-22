# Donations tables
* Scope: Owner (Drafter of the donation pays the RAM cost for the donations table)

* Schema:

#### Struct Donations
| Name of column | Data type | Description |
| ----------- | ----------- | ----------- |
| contractID | eosio::name | Unique ID of this donation contract in this scope |
| memoSuffix | eosio::name | Portion of the memo specified by the drafter used when this payment contract is executed |
| drafterPaysSignerRAM | bool | If 'true', then the drafter will pay the cost of the RAM consumed when users sign this contract |
| signerData | std::vector<SignerData> | A vector of structures that store the data relevant to the execution of this contract for each signer |

#### Struct SignerData
| Name of column | Data type | Description |
| ----------- | ----------- | ----------- |
| quantity | eosio::asset | The amount and type of token used in this donation contract |
| frequency | uint32_t | The number of seconds between each time this contract should be serviced |
| signerMemo | std::string | Portion of the memo specified by the signer used when this payment contract is executed |

* Example:

| contractID  | memoSuffix | drafterPaysSignerRAM | signerData |
| -----------  | ----------- | ----------- | ----------- |
| myDonation  | "Thanks, from MyCompany!" | false | { [ {"1.0000 EOS", 82800, "Donation to MyCompany"}, ... ] } |

