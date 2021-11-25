# Data Needed by Contract Type

## Parts of a Transaction

1. From
2. To
3. Amount
4. Frequency
5. uniqueID

## Drafter

|            | Donation | Subscription | Invoice | Outvoice | Utility |
| ---------- | -------- | ------------ | ------- | -------- | ------- |
| Implied    |          | 2            | 2,4     | 1,4      | 2       |
| Explicit   |          | 3,4,5        | 1,3     | 2,3      | 3       |
| Calculated |          |              | 5=1     | 5=2      |         |

## Signer

|            | Donation | Subscription | Invoice | Outvoice | Utility |
| ---------- | -------- | ------------ | ------- | -------- | ------- |
| Implied    | 1        | 1            |         |          | 1       |
| Explicit   | 2,3,4    | 5            |         |          |         |
| Calculated | 5=2      |              |         |          | 5=1     |
