/*| doc_header |*/

/*| doc_concepts |*/
## Error Handling

There are some cases where the RTOS may detect an error state.
The exact handling of error states is system dependent.
When an error state is detected, the RTOS calls the system-provided function `fatal_error`.
The provided `fatal_error` function takes a single [<span class="api">ErrorCode</span>] parameter, and must not return.
The `fatal_error` function can not call any of the RTOS functions.
It is expected that the function simply logs the error code in some manner, and then reset the device.

/*| doc_api |*/
## Errors

### <span class="api">ErrorCode</span>

An [<span class="api">ErrorCode</span>] refers to a specific error condition.
A value of [<span class="api">ErrorCode</span>] type is passed to a `fatal_error` function in the case where the RTOS detects an error condition.

/*| doc_configuration |*/

/*| doc_footer |*/
