/*| doc_header |*/

/*| doc_concepts |*/
# Concepts

This chapter introduces the concepts that form part of the RTOS.
The concepts defined in this chapter should be familiar to readers who have experience in other real-time operating systems.
Although most realtime operating systems provide a similar set of concepts and primitives the details for each often differ in significant aspects so the chapter is recommended for all readers.

This chapter describes the RTOS at a relatively abstract level.
RTOS training is also available and provides a more hands-on, practical introduction to the RTOS.

/*| doc_api |*/
# API Reference

This chapter provides a description of the runtime application programming interface (APIs) provided by the RTOS.

## Incorrect API Usage

It is important that the programmer correctly follow the API as described;
calling API functions with incorrect parameters must be avoided by the programmer.
The RTOS may be configured with API usage error detection enabled.
Incorrect use of the API results in the `fatal_error` function being called.

## Types

The RTOS defines a number of *types* that are used in the API.
Most types are implemented as unsigned integers (e.g: `uint8_t`).
Unfortunately the C language only performs type-checking on the underlying types, ignoring any type definitions (`typedef`s).
It is recommended that a suitable static analysis or lint tool is used to ensure that application code is using the correct types when calling APIs.

## Constant Definitions

The RTOS defines a number of pre-processor macros as constants.
Ideally these would be made available as typed static constant variables, however the compiler does not always provide optimal code generation in that case, so a pre-processor macro is used instead.

/*| doc_configuration |*/
# Configuration Reference

This chapter provides a description of the configuration interface for the RTOS.

The RTOS is configured using the `prj` tool[^prj_manual].
The RTOS is available as a `prj` module.

[^prj_manual]: See `prj` manual for more details.

The configuration items can be specified using a number of different types.
Boolean values are either true or false.
Integers are numbers of arbitrary size specified in standard base-10 notation.
C identifiers are strings that are valid identifiers in the C language (and generally refer to a function or variable that is available in the final system).
Finally, identifiers, are strings used to name the different RTOS objects.
Identifiers must be lower-case string consisting of ASCII letters, digits and the underscore symbol.
Identifiers must start with a lower-case ASCII letter.

/*| doc_footer |*/
