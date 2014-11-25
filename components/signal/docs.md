/*| doc_header |*/

/*| doc_concepts |*/
## Signals

The signal mechanism is a flexible feature that helps tasks to interact with each other.
Signals are typically used for event notifications.
For example, a driver task can use a signal to notify a handler task whenever new data is available for processing.
Typically, the driver task would send a signal every time new data becomes available while the handler task would repeatedly wait to receive the signal and then process the data.

The signal mechanism typically blocks the handler task while it waits, and makes the task runnable again when it receives the exact signal it is waiting for.
Interrupt service routines can also send signals to a task via the interrupt event mechanism described in the [Interrupt Events] section.

Each task is associated with a set of pending signals.
When the system starts, this set is empty for all tasks, so there are no signals to receive for any task.
*Sending* a signal to a task effectively adds the given signal to the task's pending signal set.
Similarly, *receiving* a signal removes the given signal from the task's pending signal set (which, of course, requires the signal to be pending in the first place).


Tasks use signals via the following main APIs:

- [<span class="api">signal_send</span>] sends a signal to another task, adding it to that task's pending signal set.

- [<span class="api">signal_peek</span>] checks whether a signal is currently pending for the calling task, i.e., whether the specified signal is in the task's pending signal set.
[<span class="api">signal_peek</span>] only performs the check without modifying the task's pending signals or blocking the task.

- [<span class="api">signal_poll</span>] checks for and receives a signal, i.e., if the given signal is in the calling task's pending signal set, it removes it.

- [<span class="api">signal_wait</span>] waits to receive a given signal, i.e., it blocks the calling task if necessary until the signal is pending and then receives it.

The above API functions also exist in versions that send or receive multiple signals in a single call.
When a task receives multiple signals, it is its responsibility to process each of those signals according to their semantics.

The RTOS does not predefine which signals exist in a system or what their meanings or semantics are.
System designers and applications are entirely free to define signals and to attach meaning to them.
They do so via the system configuration (see [Signal Configuration]).

The main property of each signal is its name, a so-called *label*, and applications always refer to signals via their names/labels.
For this purpose, the RTOS defines the constants [`SIGNAL_ID_<label>` and `SIGNAL_SET_<label>`] for each signal.
They refer to individual signals by their names and they can be used to construct signal sets from individual signals.
Although signals and signal sets are internally represented as numbers, applications neither need nor should make assumptions about those numeric values of signals.

Also see the [Signal Scopes] Section for more information on the set of tasks with which signals may be used.

/*| doc_api |*/
## Signal API

### <span class="api">SignalSet</span>

A [<span class="api">SignalSet</span>] holds multiple distinct signals (the maximum supported signals depends on the [`signalset_size`] configuration parameter).
The underlying type is an unsigned integer of the appropriate size.

### <span class="api">SignalId</span>

A [<span class="api">SignalId</span>] is an alias for the [<span class="api">SignalSet</span>] type, but represents the special case where the set contains exactly one signal (i.e., it is a singleton set).

To test whether a signal set contains a signal, use the bitwise *and* operator.
For example, `SIGNAL_SET_ALL & SIGNAL_ID_<label>` always evaluates to `SIGNAL_ID_<label>` for all signals.
To construct a signal set from multiple signals, use the bitwise *or* operator.
For example, `SIGNAL_SET_EMPTY | SIGNAL_ID_<label>` always evaluates to `SIGNAL_ID_<label>` for all signals.

### <span class="api">SIGNAL_SET_EMPTY</span>

[<span class="api">SIGNAL_SET_EMPTY</span>] has the type [<span class="api">SignalSet</span>] and represents an empty set.

### <span class="api">SIGNAL_SET_ALL</span>

[<span class="api">SIGNAL_SET_ALL</span>] has the type [<span class="api">SignalSet</span>] and represents set containing all signals in the system.

### `SIGNAL_ID_<label>` and `SIGNAL_SET_<label>`

`SIGNAL_ID_<label>` has the type [<span class="api">SignalId</span>].
`SIGNAL_SET_<label>` has the type [<span class="api">SignalSet</span>].
Constants of this form are automatically generated for each signal label.
The `<label>` portion of the name is an upper-case conversion the signal label.

### <span class="api">signal_wait_set</span>

<div class="codebox">SignalSet signal_wait_set(SignalSet requested_set);</div>

This API makes a task wait for and receive a set of signals.
If none of the signals in `requested_set` is pending, the calling task blocks until at least one of them is pending.
When the API returns, it returns all of the task's pending signals that are in `requested_set` and atomically removes them from the pending signal set.
The returned signal set is guaranteed to not be empty and to only contain signals in the requested set.
Immediately after the API returns, it is guaranteed that none of the signals in `requested_set` are pending any more.

If a signal is available immediately (without needing to block), this API implicitly calls [<span class="api">yield</span>] before returning to the user.

### <span class="api">signal_wait</span>

<div class="codebox">void signal_wait(SignalId requested_signal);</div>

This API behaves exactly as [<span class="api">signal_wait_set</span>] for the singleton signal set `requested_signal`.
Since its only valid return value would be equivalent to `requested_signal`, it does not return a value.

### <span class="api">signal_poll_set</span>

<div class="codebox">SignalSet signal_poll_set(SignalSet requested_set);</div>

This API receives a set of signals without waiting for them to become pending.
When the API returns, it returns all the task's pending signals that are in `requested_set` and atomically removes them from the pending signal set.
The returned signal set is guaranteed to only contain signals in the requested set.
Immediately after the API returns, it is guaranteed that none of the signals in `requested_set` are pending any more.
If none of the requested signals are pending, the API returns [<span class="api">SIGNAL_SET_EMPTY</span>].

This API does not implicitly call [<span class="api">yield</span>].

### <span class="api">signal_poll</span>

<div class="codebox">bool signal_poll(SignalId requested_signal);</div>

This API behaves exactly as [<span class="api">signal_poll_set</span>] for the singleton signal set `requested_signal`.
It returns true if the requested signal is pending and false otherwise.

### <span class="api">signal_peek_set</span>

<div class="codebox">SignalSet signal_peek_set(SignalSet requested_set);</div>

This API checks if a set of signals is currently pending without modifying the pending signals.
When the API returns, it returns all the task's pending signals that are in `requested_set`.
The returned signal set is guaranteed to only contain signals in the requested set.
When the API returns, it is guaranteed that all of the signals in the returned signal set are pending.
If none of the requested signals are pending, the API returns [<span class="api">SIGNAL_SET_EMPTY</span>].

This API does not implicitly call [<span class="api">yield</span>].

### <span class="api">signal_peek</span>

<div class="codebox">bool signal_peek(SignalId requested_signal);</div>

This API behaves exactly as [<span class="api">signal_peek_set</span>] for the singleton signal set `requested_signal`.
It returns true if the requested signal is pending and false otherwise.

### <span class="api">signal_send_set</span>

<div class="codebox">void signal_send_set(TaskId destination, SignalSet send_set);</div>

This API adds the signal set `send_set` to the pending signal set of the task with the ID `destination`.
After this API returns, it is guaranteed that the destination task can successfully use the peek, poll, or wait APIs to check for and/or receive the signals in `send_set`.

The API does not implicitly call [<span class="api">yield</span>].
The caller must explicitly yield if that is the intended behavior.

### <span class="api">signal_send</span>

<div class="codebox">void signal_send(TaskId task, SignalId signal);</div>

This API behaves exactly as [<span class="api">signal_send_set</span>] for the singleton signal set `signal`.

/*| doc_configuration |*/
## Signal Configuration

### `signalset_size`

This specifies the size of signal sets in bits.
It should be 8, 16 or 32.
This is an optional configuration item that defaults to 8.

### `signal_labels`

The `signal_labels` configuration is an optional list of signal configuration objects that defaults to an empty list.

### `signal_labels/signal_label/name`

This configuration specifies the name of the signal label.
Signal label names must be unique.
The name must be of an identifier type.
This configuration is mandatory.

/*| doc_footer |*/
