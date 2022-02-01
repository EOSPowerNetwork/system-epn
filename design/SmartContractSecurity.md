# Smart Contract Security
The EOS Power Network is pioneering EOS smart contract development using the [CLSDK](https://eoscommunity.github.io/clsdk-docs/book/index.html) by the Clarion team rather than the CDT released by block.one. Because of the capabilities made possible by the CLSDK, the EPN uses test-driven development (TDD) practices to aid in contract security. The following is an overview of practices in use by the EPN smart contracts that are intended to ensure the smart contracts are secure.

## Test-driven development
All tests for a new action are written before the action is implemented. This means security assumptions are challenged and asserted up front, and then the remaining effort is to implement the action such that it conforms the expectations of the test. Many companies and best-practices materials advocate for test-driven development due to its many benefits on improving design and code quality. If you're interested, you can start learning more about its benefits [here](https://www.codica.com/blog/test-driven-development-benefits/).

## BDD-style testing
[Catch2](https://github.com/catchorg/Catch2), which is the testing framework available for use by smart contracts using CLSDK, allows for [BDD-style test cases](https://github.com/catchorg/Catch2/blob/devel/docs/test-cases-and-sections.md#bdd-style-test-cases) to be written. Using ths style of test writing:
1. Eliminates a lot of redundant set-up code
2. Makes test cases easier to read
3. Makes test cases easier to write

This helps ensure maximal test coverage, and therefore higher assurance of code quality.

## Don't test parameter validity in your action implementation
The EPN contends that it should be a standard practice for smart contract developers to wrap action parameters in custom types if there are any restrictions on the value of the parameter. For example, when signing a donation (an action in the EPN system contract), the signer specfies the frequency with which they want to donate.

Bad design: Make the frequency parameter an int64_t and check inside the action that the value of the passed parameter is within specific minimum and maximum frequency.
Good design: Wrap the int64_t in a Frequency data type, and validate the value contraints as part of the Frequency constructor.

When using the preferred design above, then only a couple test cases must be written to check that the construction of the Frequency data type adheres to the intended constraints, rather than including these test cases on every action that takes a frequency parameter. This also helps eliminate redundant error-checking code that could otherwise clog up action implementation, diminishing code readability.

## Abstract the complexity of working with RAM
Multi-index tables, KV tables, Singletons, even [VRAM](https://liquidapps.io/vram) are all mechanisms by which smart contracts can store and retrieve data necessary for their operation. The smart contracts themselves need data, and an interface should be built to provide that data that hides the complexity of how that data access and mutation is achieved. With  properly separated interfaces, unit tests and smart contract code can both share the interface to minimize redundancy and improve code quality.

These data interfaces should also manage all the complexity of checking the validity of state updates before applying them.

## Keep contracts lean
If you test parameter validity inside data wrappers, and you keep state update validity checks in the data management interfaces, then your smart contracts will be extremely lean. The main responsibility of a smart contract is just to serve as an API call, check that the caller is authorized to make the call, and then dispatch a state update or retrieval to the data interfaces.
