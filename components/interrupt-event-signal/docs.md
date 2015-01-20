/*| provides |*/
interrupt-event-signal

/*| requires |*/
task
signal
preempt
interrupt-event

/*| doc_header |*/

/*| doc_concepts |*/
## Interrupt Events

Interrupt events provide the bridge between tasks and interrupt service routines.
The system can be configured with a number of interrupt events.
Each interrupt event is associated with a [<span class="api">TaskId</span>] and [<span class="api">SignalSet</span>][^signalset].
The [<span class="api">TaskId</span>] and [<span class="api">SignalSet</span>] association is usually done when the system is configured.
A task may choose to update this association at run-time using the [<span class="api">interrupt_event_task_set</span>] API.

[^signalset]: See the [Signals] section for more details.

An interrupt service routine can call the [<span class="api">interrupt_event_raise</span>] API to raise one of the interrupt events configured in the system.
The [<span class="api">interrupt_event_raise</span>] API is carefully implemented using an atomic instruction to avoid any possible data races.
When an interrupt event is raised, it causes the associated signal set to be sent to the task associated with the interrupt event.
This provides a safe and efficient mechanism for interrupt service routines to interact with tasks.

/*| doc_api |*/
### <span class="api">interrupt_event_task_set</span>

<div class="codebox">void interrupt_event_task_set(InterruptEventId interrupt_event_id, TaskId task_id);</div>

This function configures at run time which task is signaled when the specified interrupt event is raised.
In some application scenarios, the static configuration via [`interrupt_events/interrupt_event/task`] is sufficient and does not need to be modified at run time.
In those cases, this function does not need to be called.
In other scenarios, it an interrupt event may need to be signaled to a dynamically changing set of tasks.
In those cases, this function allows the application to update this configuration setting at run time.


/*| doc_configuration |*/
### `interrupt_events/interrupt_event/task`

This configuration item specifies the task to which a signal set is sent when the interrupt event is raised.
This configuration item is optional.
If no task is set, raising the interrupt event causes a fatal error.
If the system designer does not set the task in the static configuration, it can be set at runtime using the [<span class="api">interrupt_event_task_set</span>] API.

### `interrupt_events/interrupt_event/sig_set`

This configuration item specifies the signal set that is sent to the interrupt event's associated task.
A signal set is a list of one or more specified signal labels.
This configuration item is optional and defaults to the empty set.

/*| doc_footer |*/
