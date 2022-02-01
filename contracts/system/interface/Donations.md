# Donations interfaces

Multi-index tables related to the drafting and signing of Donation contracts.

## Interface: DonationContract

A stateful object that can be created with the information necessary to uniquely identify a contract, namely the drafter and the contractID. Once created, it aggregates the draft of the contract ([DonationDraft](../schema/DonationDraft.md) object) and a vector containing all signatures ([DonationSignature](../schema/DonationSignature.md) object). and provides a helpful interface to access the various properties of this contract.

## Interface: DonationsIntf

A stateless object used to query any information about Donation contracts. Often constructs a DonationContract and forwards the request.
