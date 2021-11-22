# EPN System Tests
With the development of the high-speed testing capabilities of the CLSDK, a new era of thoroughness in smart contract testing is available for those working on EOSIO blockchain applications.

The EOS Power Network uses test-driven development (TDD) via the CLSDK as a standard operating procedure to help ensure the correctness of all state managed by the EOS Power Network. 

## Test suite: Donations

### Action(s): DraftDon

1. Verify that a donation is able to be drafted & state is accurate
2. Verify that the same owner can create two contracts with different contractIDs
3. Verify that the same owner cannot create two contracts with the same contractID
4. Verify that the owner must be a signer on the draft
5. Verify that two different drafters can use the same contractID
6. Verify that the drafter memo cannot exceed fixedProps::memoSize

### Action(s): SignDon

7. Verify that a donation can be signed & state is accurate
8. Verify that the RAM payer matches the payer specified by the drafter
9. Verify that the signer has authorized the sign action
10. Verify that the donation being signed actually exists
11. Verify that the signer cannot sign the same donation twice
12. Verify that drafter cannot sign their own donation
13. Verify that multiple unique signers may sign a donation
14. Verify that only the accepted token symbols can be used to donate
15. Verify that only frequencies in the accepted range (min/max) can be used
16. Veriify that the signer memo cannot exceed fixedProps::memoSize
17. Verify that the first donation goes out immediately
18. Verify that the second donation goes out correctly
