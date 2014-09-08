/*| provides |*/
signal-task
signal-scope

/*| requires |*/
rtos
signal

/*| doc_header |*/

/*| doc_concepts |*/
## Signal Scopes

The RTOS represents signals and signal sets as bit masks of the [<span class="api">SignalSet</span>] type.
With a growing number of signals, the [<span class="api">SignalSet</span>] type may become too narrow to allocate distinct bits for all signals.
To mitigate this issue, the RTOS distinguishes between *global signals* and *task signals* which allows for optimizing the bit allocation in [<span class="api">SignalSet</span>].

### Global vs. Task Signals

*Global signals* are signals which are handled in multiple different tasks whereas a *task signal* is handled only by a small fixed set of tasks known at configuration time.

A typical example of a global signal is one that a driver task sends to client tasks to inform them that the driver has new data available for those clients.
Since those clients can be any number of tasks that are not necessarily known at configuration time, the signal in question is best configured as a global one.

Conversely, a common instance of a task signal is a signal that client tasks send to a driver task to notify the driver of client data being available.
In this case, only a single fixed task is known to handle that signal.
Since that makes for a small fixed set of handler tasks (one in this case) known at configuration time, that signal is best [configured](#task-signal-configuration) as a task signal.

### Impact on Signal IDs

Task signals allow the RTOS to optimize the allocation of bits for signal IDs and sets.
In particular, it can use fewer bits for signals that are sent to disjoint task sets.
This may allow the application to configure and use a smaller, more efficient type for signal IDs and sets.

### Impact on System Configuration and Implementation

For application correctness, it is crucial that task signals are [configured](#task-signal-configuration) and used correctly.
The system configuration needs to be correct in the sense that each task signal is configured for the correct set of tasks, i.e., all tasks that receive and handle that signal.
The application implementation needs to be correct in the sense that each task signal is sent only to and handled only in the set of tasks configured in the system configuration.

For correctness, it is necessary and sufficient that the implementation does not send a task signal to a task outside the configured task set.
For efficiency, it is recommended to configure signals as task signals with as small task sets as possible.
It is always safe but potentially inefficient to configure all signals as global signals or to configure task signals with unnecessarily large task sets.

/*| doc_api |*/

/*| doc_configuration |*/
## Task Signal Configuration

### `signal_labels/signal_label/global`

This optional boolean configuration item defaults to true.
If true, the given signal label is considered to refer a [global signal](#signal-scopes).
If true, the [`signal_labels/signal_label/tasks`] configuration item may not be used for the same signal label.
If false, the [`signal_labels/signal_label/tasks`] configuration item is required.

### `signal_labels/signal_label/tasks`

This optional list configuration item makes a signal a [task signal](#signal-scopes).
It defaults to the empty list.
If present, it needs to contain a non-empty set of `signal_labels/signal_label/tasks/task` configuration items that identify the tasks that the given signal is sent to.
If this list is present, the [`signal_labels/signal_label/global`] configuration item may not be set to true and is implicitly set to false.
If this configuration item is not present, the [`signal_labels/signal_label/global`] configuration item needs to be left at its default or explicitly set to true.

### `signal_labels/signal_label/tasks/task`

Each of these identifier configuration items needs to contain the name of a task that the signal `signal_label` is sent to and handled by.
See [Signal Scopes] for more information on global and task signals.

/*| doc_footer |*/
