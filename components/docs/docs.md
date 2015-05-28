/*| provides |*/
docs

/*| requires |*/

/*| doc_header |*/

/*| doc_concepts |*/
# Concepts

This chapter introduces the concepts that form the foundation of the RTOS.
Many of these concepts would be familiar to readers who have experience in other real-time operating systems.
However, this chapter does cover aspects that are specific to the RTOS and significant for constructing correct systems.

In addition to this documentation, RTOS training can provide a more hands-on, practical introduction to the RTOS.

/*| doc_api |*/
# API Reference

This chapter describes the runtime application programming interface (APIs) provided by the RTOS.

## Correct API Usage

The RTOS API design and implementation leave room for the application to use it incorrectly.
For example, there are generally no safeguards in the RTOS itself against the application supplying invalid task-ID values to API functions.
To achieve correct system behavior, it is the application's responsibility to use the RTOS API under the conditions and requirements documented here.

The RTOS implementation is able to detect some case of the application using the RTOS API incorrectly.
This feature needs to be enabled in the system configuration (see [`api_asserts`]).
When enabled, the RTOS checks some but not all cases of the API being used incorrectly.
If it detects such a case, it calls the `fatal_error` function.

## Types

The RTOS defines a number of *types* that are used in the API.
Most types are implemented as unsigned integers (e.g: `uint8_t`).
Unfortunately, the C language performs type checking only on the underlying types, but ignores any type definitions (`typedef`s).
It is recommended that a suitable static analysis or lint tool is used to ensure that application code is using the correct types when calling APIs.

## Constant Definitions

The RTOS defines a number of pre-processor macros as constants.
Ideally these would be made available as typed static constant variables, however some compilers do not always generate optimal code for that case, so a pre-processor macro is used instead.

/*| doc_configuration |*/
# Configuration Reference

This chapter provides a description of the available configuration items for the RTOS.
The RTOS is configured via the configuration tool called `prj`[^prj_manual].

[^prj_manual]: See `prj` manual for more details.

### Types

Configuration items have one of a number of different types.

- Boolean values are either true or false.

- Integers are numbers of arbitrary size specified in standard base-10 notation.

- C identifiers are strings that are valid identifiers in the C language (and generally refer to a function or variable that is available in the final system).

- Identifiers are strings used to name the different RTOS objects.
Identifiers must be lower-case string consisting of ASCII letters, digits and the underscore symbol.
Identifiers must start with a lower-case ASCII letter.

### Notation

Since the system configuration is specified in XML format, the configuration items form a hierarchy of elements.
For example, the list of tasks contains an entry for each task where in each entry several task-specific items are nested.

For clarity, this document refers to each item through a notation similar to [XPath](http://www.w3.org/TR/xpath).
For example, the *name* property of a task in the tasks component would be referred to as *tasks/task/name*.
It reflects not only the name of the property itself but also its location within the hierarchy of an entire system definition.


/*| doc_footer |*/
