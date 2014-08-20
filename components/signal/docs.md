/*| doc_header |*/

/*| doc_concepts |*/
## Signals

Signals provide a low-level mechanism for controlling when tasks are runnable and when they are blocked.
A task can wait to receive a signal, and it becomes runnable when another task (or an interrupt handler) sends it a signal it is waiting for.

A signal is a small integer value in a range from zero up to 31.
Signals are always grouped together in signals sets that are collections of distinct signals.
Internally a signal set is represented as a bit field on a standard 8, 16 or 32-bit integer (as chosen by the system designer).

Apart from a small set of built-in signal numbers the RTOS does not specify the semantics of the signal numbers;
it is up to the signal designer to specify the meaning of a signal in the context of a particular task.
For example, when task A receives signal 5, it may mean task A should start sending a communications packet, however when task B receives the same signal, it may mean turn off an LED.

To assist the system designer in allocating signal IDs in an optimal manner, the RTOS configuration tool allows the system designer to define a list of signal labels.
For example, in the previous example the system designer may define the labels `send_packet` and `led_off`.
The system designer also specifies which signal labels each task is interested in (i.e.: they would associate `send_packet` with task A, and `led_off` with task B).
Some signal labels may apply to multiple tasks (global signal labels) while other signal labels may only apply to a single task.
The configuration tool provides macros in the form `SIGNAL_ID_<signal_label>`, that map to a singleton set containing the labelled signal.
It is important to note that multiple different labels could refer to the same signal.
In the previous example, `SIGNAL_ID_SEND_PACKET` and `SIGNAL_ID_LED_OFF`, would both label signal number 5.

The RTOS maintains a pending signal set for each task.
Tasks can send a set of signals to another task using the [<span class="api">signal_send</span>] API.
This operation delivers the signals to the receiving task by adding the signals to the receiving task's pending signal set.
If a task receives a signal that is already pending then it has no effect on the pending signal set.
Interrupt service routines can also send a signal set to a task via the interrupt event mechanism described in the previous section.

Signals remain in a task's pending signal set until they are received by the task.
The task can receive a set of signals by using the wait or poll operations.
Additionally, a task can check which signals are in the pending signal set by using the peek operation.
As a task can receive multiple signals in a single RTOS call it is the responsibility of the task to correctly process each of the tasks received.
The programmer can choose the order in which the received signals are processed.

The poll operation ([<span class="api">signal_poll_set</span>]) allows a task to receive a set of signals without blocking.
The task provides a requested signal set to the operation and the RTOS returns a signal set that contains any signals that are in both the requested signal set and the task's pending signal set (i.e.: the intersection of the two sets).
All the signals in the returned signal set are removed from the pending signal set.
If none of the requested signals are currently in the pending set then the empty set is returned.

The peek operation ([<span class="api">signal_peek_set</span>]) allows a task to determine which signals are currently pending without removing the signals from the pending set.
As with the poll operation the task supplies a requested signal set, and any signals that are both in the pending set and the requested set are returned, however the returned signals are not removed from the pending set.

The wait operation ([<span class="api">signal_wait_set</span>]) allows a task to block until a requested signal is available.
As with poll (and peek) the task provides a requested signal set to the wait operation, and returns the set of signals that is both in the requested and pending sets.
If none of the requested signals are pending then rather than returning the empty set the task blocks until at least one of the requested signals is available.

/*| doc_api |*/
## Signals

### <span class="api">SignalSet</span>

A [<span class="api">SignalSet</span>] holds multiple (up to 8, 16, or 32 depending on RTOS configuration) distinct signals.
The underlying type is an unsigned integer of the appropriate size.

### <span class="api">SignalId</span>

A [<span class="api">SignalId</span>] is an alias for the [<span class="api">SignalSet</span>] type, but represents the special case where the set contains exactly one signal (i.e: is a singleton set).
The RTOS configuration tool creates constants of the form `SIGNAL_ID_<name>` for any global signal labels, and `SIGNAL_ID_<task>_<name>` for any task-local signal labels.

### <span class="api">SIGNAL_SET_EMPTY</span>

[<span class="api">SIGNAL_SET_EMPTY</span>] has the type [<span class="api">SignalSet</span>] and represents an empty set.

### <span class="api">SIGNAL_SET_ALL</span>

[<span class="api">SIGNAL_SET_ALL</span>] has the type [<span class="api">SignalSet</span>] and represents set containing all possible signals.

### `SIGNAL_ID_<label>`

`SIGNAL_ID_<label>` has the type [<span class="api">SignalId</span>].
A constant of this form is created for each global signal label.
The *label* portion of the name is an upper-case conversion the signal label.

### `SIGNAL_ID_<task-name>_<label>`

`SIGNAL_ID_<task-name>_<label>` has the type [<span class="api">SignalId</span>].
A constant of this form is created for each task-local signal label.
The *label* part of the name is an upper-case conversion the signal label.
The *task-name* part of the name is an upper-case conversion of the task name.

### <span class="api">signal_wait_set</span>

<div class="codebox">SignalSet signal_wait_set(SignalSet requested_set);</div>

The [<span class="api">signal_wait_set</span>] API allows a task to wait on a set of signals.
The calling task blocks until one of the signals in the requested_set is available.
The function returns a set of received signals.
The returned signal set is guaranteed to not be empty and to only contain signals in the requested set.
If a signal is available immediately (without needing to block) this API performs a yield before returning to the user.

### <span class="api">signal_wait</span>

<div class="codebox">void signal_wait(SignalId requested_signal);</div>

The [<span class="api">signal_wait</span>] API allows a task to wait for a single requested signal.
The calling task blocks until the requested signal is available.
If a signal is available immediately (without needing to block) this API performs a yield before returning to the user.
Note: this is a simple wrapper around the underlying [<span class="api">signal_wait_set</span>] API.

### <span class="api">signal_poll_set</span>

<div class="codebox">SignalSet signal_poll_set(SignalSet requested_set);</div>

The [<span class="api">signal_poll_set</span>] API works in a similar manner to the [<span class="api">signal_wait_set</span>] API, however instead of blocking if a signal is unavailable the API returns immediately.
An empty set is returned if none of the requested signals are available.
The [<span class="api">signal_poll_set</span>] APIs does not cause a yield operation to occur.

### <span class="api">signal_poll</span>

<div class="codebox">bool signal_poll(SignalId requested_signal);</div>

The [<span class="api">signal_poll</span>] API work in a similar manner to the [<span class="api">signal_wait</span>] API, however instead of blocking if a signal is unavailable the API returns immediately.
The function returns true if the requested signals are available (and false otherwise).
The [<span class="api">signal_poll</span>] APIs does not cause a yield operation to occur.
Note: This is a simple wrapper around the [<span class="api">signal_poll_set</span>] API.

### <span class="api">signal_peek_set</span>

<div class="codebox">SignalSet signal_peek_set(SignalSet requested_set);</div>

The [<span class="api">signal_peek_set</span>] API can be used to determine if a set of requested signals is currently pending.
A signal set of the currently available signals is returned.
If no signals in the requested set are available then an empty set is returned.
Unlike the [<span class="api">signal_poll_set</span>] API, the returned signals are not received (i.e.: the returned signals are not removed from the task's pending set).
This function does not imply a yield operation.

### <span class="api">signal_peek</span>

<div class="codebox">bool signal_peek(SignalId requested_signal);</div>

The [<span class="api">signal_peek</span>] API can be used to determine if an individual signal is available for delivery.
If the signal is available, the function returns true (otherwise false is returned).
Unlike the [<span class="api">signal_poll</span>] API, the signal is not received (i.e.: the signal is not removed from the task's pending set).
This function does not imply a yield operation.
Note: this function is a simple wrapper around the underlying [<span class="api">signal_peek_set</span>] API.

### <span class="api">signal_send_set</span>

<div class="codebox">void signal_send_set(TaskId destination, SignalSet send_set);</div>

The [<span class="api">signal_send_set</span>] API adds the signal set to the destination task's pending signal set.
The API does not cause a yield operation to occur.
The caller must explicitly yield if that is the intended behaviour.

### <span class="api">signal_send</span>

<div class="codebox">void signal_send(TaskId task, SignalId signal);</div>

The [<span class="api">signal_send</span>] API adds the signal to the destination task's pending signal set.
The API does not cause a yield operation to occur.
The caller must explicitly yield if that is the intended behaviour.
Note: this function is a simple wrapper around the underlying [<span class="api">signal_send_set</span>] API.

/*| doc_configuration |*/
## Signal Configuration

### `signalset_size`

This specifies the size of signal sets.
It should be 8, 16 or 32.
This is an optional configuration item that defaults to 8.

### `signal_labels`

The `signal_labels` configuration is a list of signal label configuration objects.

### `name`

This configuration specifies the name of the signal label.
Signal label names must be unique.
The name must be of an identifier type.
This configuration is mandatory.

/*| doc_footer |*/
