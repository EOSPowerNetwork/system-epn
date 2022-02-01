# DonationSignature

| Name of column | Data type | Description |
| ----------- | ----------- | ----------- |
| index | uint64_t | 0-based index of the contract signature, increments with each new signature |
| signer | eosio::name | The account name of the signer |
| contractID | eosio::name | The name of the contract to which this signature applies |
| serviceBlock | eosio::block_timestamp | The block height at or after which the terms of this contract may be executed |
| drafter | eosio::name | The name of the account that drafted the contractID. Needed because contractIDs are only unique within a drafter scope. |
| quantity | eosio::asset | The amount and type of token used in this donation contract |
| frequency | uint32_t | The number of seconds between each time this contract should be serviced |
| signerMemo | std::string | Portion of the memo specified by the signer used when this payment contract is executed |

## Example

| index | signer | contractID | serviceBlock | drafter | quantity | frequency | signerMemo |
| -----------  | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| 0 | "bob"_n | "mydonation"_n | 213478929 | "alice"_n | 1.0000 EOS | 82800 | "Donation to Alice" |
