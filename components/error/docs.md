/*| provides |*/
error

/*| requires |*/
rtos
/*| doc_header |*/

/*| doc_concepts |*/
## Error Handling

There are some rare cases in which the RTOS may detect a fatal error state.
Fatal errors include, for example, the fact that the timer component is unable to process a timer tick before another tick occurs.
How such error states are handled best depends to a large degree on the application and system requirements.
Therefore, the RTOS allows applications to customize the response to error states.

When the RTOS detects an error state, it calls the application-provided function [`fatal_error`].
The application can implement its preferred strategy to deal with error states this way, where typical approaches are to log the error state and reset the system.
The RTOS supplies a single [<span class="api">ErrorId</span>] parameter to the application's [`fatal_error`] function that indicates the kind of error that occurred.

The RTOS relies on [`fatal_error`] to not return, so it must stop the system and prevent the RTOS and application from continuing to execute.
This also means that the [`fatal_error`] function must not use any RTOS APIs.

### Assertions

A particular instance of error states is when an assertion in the RTOS implementation fails.
There two kinds of such assertions:
API assertions and internal assertions.
Both kinds are optional and can be enabled or disabled in the system configuration (see [Error Handling Configuration]).

API assertions check a wide range of (but not all) requirements and preconditions that the RTOS API has regarding application run-time behavior.
For example, the [<span class="api">mutex_lock</span>] API implementation can assert that the mutex ID that the application provides identifies a valid mutex.
All such assertions cover requirements and preconditions that are clearly documented.
It is good practice for applications to enable these assertions via the [`api_asserts`] configuration item for debugging and test builds.
It is recommended to disable them in release builds to avoid the code-size and execution-time costs they incur.

Internal assertions check a (relatively small) range of internal assumptions of the RTOS implementation that are not related to the application behavior.
These implementation-specific sanity checks are irrelevant to applications and primarily intended for testing and debugging the RTOS implementation itself.
Applications are free to enable them via the [`internal_asserts`] configuration item.
However, they provide no tangible benefit to applications and they do incur code-size and execution-time overhead.


/*| doc_api |*/
## Error Handling API

### <span class="api">ErrorId</span>

An instance of this type refers to a specific RTOS error state.
When the RTOS detects an error state, it passes a value of type [<span class="api">ErrorId</span>] to the [`fatal_error`] function, which the application needs to implement.
The RTOS implementation defines constants that identify all potential error states and the corresponding [<span class="api">ErrorId</span>] values.
The names of these constants follow the pattern `ERROR_ID_<error-state>`.

/*| doc_configuration |*/
## Error Handling Configuration

### `api_asserts`

This configuration item is a boolean with a default of false.
When true, the RTOS checks the arguments passed to API functions at runtime for consistency.
For example, it checks whether the value of the [<span class="api">MutexId</span>] argument of the [<span class="api">mutex_lock</span>] API identifies a valid mutex.
If the check passes, the check has no effect on the behavior of the API.
If the check fails, the RTOS calls the [`fatal_error`] function.


### `internal_asserts`

This configuration item is a boolean with a default of false.
When true, the RTOS checks internal implementation state for consistency at runtime.
For example, it checks whether the [<span class="api">TaskId</span>] instance identifying the current task is a valid task ID.
If the check passes, the check has no effect on the behavior of the RTOS.
If the check fails, the RTOS calls the [`fatal_error`] function.


### `fatal_error`

This configuration item is an optional C identifier with no default.
It must be the name of a C function which the application implements and the RTOS calls when a fatal error state occurs.
See Section [Error Handling] for more information.

/*| doc_footer |*/
