# EPN System Tests
With the development of the high-speed testing capabilities of the CLSDK, a new era of thoroughness in smart contract testing is available for those working on EOSIO blockchain applications.

The EOS Power Network uses test-driven development (TDD) via the CLSDK as a standard operating procedure to help ensure the correctness of all state managed by the EOS Power Network. 

## Test suite: Donations

### Action(s): DraftDon

1. Verify that a donation is able to be drafted
2. Verify that the same owner can create two contracts with different contractIDs
3. Verify that the same owner cannot create two contracts with the same contractID
4. Verify that the owner must be a signer on the draft
5. Verify that two different drafters can use the same contractID

