# Cron
In order to service recurring transactions, the EOS Power Network operators must run a cron service to execute transactions according to the rules of the signed contracts under the network's management. Building a general-purpose cron-service has challenges, particularly in designing how users of the cron-service would be allocated portions of the limited bandwidth on offer for servicing cron jobs, and how they would pay for it. However, the EOS Power Network is not, at least in its first phase of deployment, a general purpose on-chain cron solution. Rather, it is a specific cron solution for token transfers. Because of this, the challenges in allocating and paying for the limited cron bandwidth are simpler to overcome.

## Resource management
The EOS Power Network operators are the ones to service the transactions, and so they must individually ensure that they have enough CPU and NET resources on the EOS blockchain to be able to execute an established maximum number of transactions per block.

## Paying for the cron-service
In the case of the EOS Power Network, a small fee will be taken from each serviced transaction and paid to the EOS Power Network. This fee serves multliple purposes:
1. It pays for the cost of computation incurred by the producers who are servicing the transaction.
2. It establishes an economic threshold for the value of transactions below which users cannot use the EPN to schedule their recurring transaction.

## Implications
The implications of this cron design are:
1. The EPN is not intended to be used for servicing micro-payments. Each supported token will have a minimum transfer quantity (enough to make the percentage-based fee worthwhile for operators), established by the operators and compiled into the system contract.
2. The EPN does not provide hard time guarantees for executing transactions. If users specify X seconds between transfers, it cannot be guaranteed that the recurring transaction will be included in the first block at which the specified number of seconds has passed. If there happen to be a large number of transactions available for execution in that block, only the first batch (final number tbd) of them will be executed in that block. Each block, the maximum number of transactions will be executed that are avialable for execution until there are none ready to execute.

## Maximum number of serviceable contracts
Therefore, there is a theoretical maximum number of contracts serviceable by the EPN. The actual maximum number is not able to be calculated with precision because it depends on the specific frequencies used when drafting contracts. But we can calculate a range for the maximum number of serviceable contracts.

Let the minimum (f<sub>min</sub>) contract frequency serviceable by the EPN be 23 hours, and the maximum (f<sub>max</sub>) be 1 year. In seconds, f<sub>min</sub> = 23 \* 60 \* 60 = 82,800, and f<sub>max</sub> = 365 \* 24 \* 60 \* 60 = 31,536,000. 

The minimum total number of serviceable contracts would be if the entire cron bandwidth was filled by contracts serviced every f<sub>min</sub> seconds, and the maximum total number of serviceable contracts would be if the entire cron bandwidth was filled by contracts serviced every f<sub>max</sub> seconds.

If the number of transactions per block (t) is 2, and the number of blocks per second (b) is 2, then our final range is:
[t\*b\*f<sub>min</sub>, t\*b\*f<sub>max</sub>] = [331200, 126144000].

As you can see, even at the minimum total number of serviceable contracts, there is a lot of bandwidth, even for a relatively low cost for EOS Power Network operators (2 transactions per block). If the EPN were ever approaching the maximum number of serviceable contracts, the operators could agree to allocate more resources to the network and increase the total number of serviced transactions per block. Each additional transaction per block would yield an a [b\*f<sub>min</sub>, b\*f<sub>max</sub>] = [165600, 63072000] increase to the range of total serviceable contracts.