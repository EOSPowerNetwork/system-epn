# EPN System Tests
With the development of the high-speed testing capabilities of the CLSDK, a new era of thoroughness in smart contract testing is available for those working on EOSIO blockchain applications.

The EOS Power Network uses test-driven development (TDD) via the CLSDK as a standard operating procedure to help ensure the correctness of all state managed by the EOS Power Network. 

Specifically, [BDD-style testing](http://dannorth.net/introducing-bdd/) is used to make it easier to read, write, and understand test logic.

## Test suite: \[Donations\]

### Data Type Tests
* Given a valid memo, a Memo object can be constructed
* Given an invalid memo, a Memo object cannot be constructed
* TODO: Verify that only frequencies in the accepted range (min/max) can be used

### Action(s): DraftDon
* Scenario 1: A single drafter using the \"draftdon\" action
  * Given an empty chain
    * Then Alice's donation should not exist
    * When Alice creates a donation
      * Then Alice's Donation should exist
      * Then Alice is the only one whose RAM is consumed
        * Also only the expected amount of RAM is consumed
      * Then a second donation may be created using a different ID
      * Then a second donation may not be created with the same ID

* Scenario 2: Two drafters are using the \"draftdon\" action
  * Given an empty chain and two potential drafters, Alice and Bob
    * Then Alice cannot create a donation for Bob without Bob's authorization
    * When Alice creates a donation
      * Then Bob can also create a donation with the same ID

### Action(s): SignDon

* TODO Scenario 1: A single signer using the "signdon" action to sign a single donation draft
  * Given a chain in which Alice drafted a donation
    * Then the donation should have zero signers
    * Then Alice cannot sign her own donation
    * Then Bob cannot sign Alice's donation with an unaccepted currency
    * Then Bob can sign Alice's donation
      * And then the donation should have exactly one signer
      * And then the RAM payer specified by the donation draft should be charged the RAM costs
        * Also only the expected amount of RAM is consumed
    * When Bob signs Alice's donation
      * Then Bob cannot sign Alice's donation again
      * Then the first donation goes out immediately
        * And then the second donation goes out correctly

* TODO Scenario 2: Two signers interacting with the "signdon" actioni to sign a single donation draft
  * Given a chain in which Alice drafted a donation, and two potential signers Bob and Charlie
    * Then Charlie cannot sign the donation for Bob without Bob's authorization
    * Then Charlie cannot sign Bob's donation (it doesn't exist)
    * When Bob signs Alice's donation
      * Then Charlie can also sign Alice's donation

* TODO Scenario 3: One signer interacting with the "signdon" action, with multiple donation drafts
  * Given a chain in which Alice and Bob drafted donations, with Charlie as a potential signer
    * When Charlie signs Alice's donation
      * Then Charlie can also sign Bob's donation

