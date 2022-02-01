# DonationDraft

* Scope: Owner (Drafter of the donation pays the RAM cost for the donations table)

| Name of column | Data type | Description |
| ----------- | ----------- | ----------- |
| contractID | eosio::name | Unique ID of this donation contract in this scope |
| memoSuffix | eosio::name | Portion of the memo specified by the drafter used when this payment contract is executed |

## Example

| contractID  | memoSuffix |
| -----------  | ----------- |
| myDonation  | "Thanks, from TheCompany!"  |
