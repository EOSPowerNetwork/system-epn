# Microforks
Microforks happen when the active producer is switching to the next one in the schedule of producers. It sometimes happens that when switching from producer A to producer B, producer B does not receive some of the blocks produced by A in time before it started producing its own blocks. When this happens, the transactions included in the blocks by A that were not recieved by B end up on a Microfork, which is a fork of the blockchain that will not be included in the canonical chain. Most of the time, this doesn't matter because block producer B will include the same transaction in their produced blocks, but occasionally a transaction will expire before B's producer slot, and those transactions therefore get forked out.

## Risk
The only risk one could imagine due to microforks is that a power network operator tries to service a contract in a block that ends up getting forked out of the canonical chain. 

## Solution 
The smart contract state that indicates that the contract was serviced will only be updated if the contract is serviced in a canonical block, and therefore microforks do not pose a risk. Any forked out transactions will simply be re-executed in a subsequent block.

