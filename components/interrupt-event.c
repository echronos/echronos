/*| dependencies |*/
error
task

/*| doc_concepts |*/
## Interrupt Service Routines

In a microcontroller based system there are two contexts in which code operates;
the normal context and the interrupt context.
By default the microcontroller executes code in the normal context however in response to interrupts the microcontroller transfers to the special interrupt context and executes a registered interrupt service routine (ISR).
When an ISR completes the microcontroller returns to the normal context and continues to execute the previously interrupted code path.

RTOS tasks execute in the normal context, and although tasks do not preempt other tasks (as explained in the Tasks section), tasks can be interrupted by ISRs.
This allows the system to respond to hardware events with low latency.
It is possible to block ISRs from interrupting tasks by disabling interrupts, however this is discouraged as it increases the latency of ISRs responding to interrupts.

ISRs execute on the stack of the current task.
For this reason the ISR should minimise its stack usage.
As the ISR may interrupt any task, each task must have a large enough stack to accommodate both its own usage and the maximum stack usage of any ISR.

As the system may interrupt a task at any time it is important that the ISR is careful with any data structures that it may share with tasks.
For example, if an ISR and a task both have code that increments a value (e.g.: `i++`), it is possible for one of the increments to be lost.
Code such as `i++` is implemented with three separate instructions: read from memory, increment value and write to memory.
If an interrupt happens between the read and write instructions any changes to the variable that the ISR makes would be lost when the tasks write instruction is executed.
One approach to solve the problem is to disable interrupts when updating data structures (e.g.: `interrupt_disable(); i++; interrupt_enable()`).
The problem with this approach is that it increases the latency of ISRs.
The alternate solution is to make use of instructions that can modify a memory location in a single operation.

An important design constraint of the RTOS is to ensure that ISRs execute with a low latency, so the RTOS avoids disabling interrupts whenever possible.
As a result an interrupt may occur during the execution of an RTOS API function, which means that the RTOS state may not be consistent when an ISR executes.
For this reason the ISRs must not call the regular RTOS APIs.
If an ISR were to call any regular RTOS function, it would corrupt RTOS state and cause incorrect system behaviour.

Of course, it is necessary for ISRs to interact with tasks.
To achieve this a special interrupt event object provides the link between ISRs and tasks.
This object is described in the following section.


## Interrupt Events

Interrupt events provide the bridge between tasks and ISRs.
The system can be configured with a number of interrupt events.
An ISR can call the [<span class="api">interrupt_event_raise</span>] API to raise a specific interrupt event.
The [<span class="api">interrupt_event_raise</span>] API is carefully implemented using an atomic instruction to avoid any possible data races.
When an interrupt event is raised it causes a signal set to be sent to the task associated with the interrupt event when the current task yields or blocks.
This provides a safe and efficient mechanism for ISRs to interact with tasks.

Each interrupt event is associated with a [<span class="api">TaskId</span>] and [<span class="api">SignalSet</span>][^signalset].
The [<span class="api">TaskId</span>] and [<span class="api">SignalSet</span>] association is usually done when the system is configured.
A task may choose to update this association at run-time using the [<span class="api">interrupt_event_task_set</span>] API.

[^signalset]: See the following section on signals for more details.

/*| doc_api |*/
## Interrupts

### <span class="api">InterruptEventId</span>

A [<span class="api">InterruptEventId</span>] refers to a specific interrupt event.
The underlying type is an unsigned integer of a size large enough to represent all interrupt events[^InterruptEventId_width].
The [<span class="api">InterruptEventId</span>] should be treated an an opaque value.
For all interrupts in the system, the configuration tool creates a constant with the name `INTERRUPT_EVENT_ID_<name>` that should be used in preference to raw values.

[^InterruptEventId_width]: This is normally a `uint8_t`.

### `INTERRUPT_EVENT_ID_<name>`

`INTERRUPT_EVENT_ID_<name>` has the type [<span class="api">InterruptEventId</span>].
A constant is created for each interrupt event in the system.
Note that *name* is the upper-case conversion of the interrupt event's name.

### <span class="api">interrupt_event_raise</span>

<div class="codebox">void interrupt_event_raise_<name>(void);</div>

The `interrupt_event_raise_<name>` API provides the interface that enables an interrupt service routine to raise an interrupt event.
This API must only be called from an interrupt service routine (not a task).
Raising an interrupt event causes the signal set associated with the interrupt event to be sent to the task associated with the interrupt event on the next yield or block.
This and [<span class="api">timer_tick</span>] are the only RTOS API functions that an interrupt service routine may call.

### <span class="api">interrupt_event_task_set</span>

<div class="codebox">void interrupt_event_task_set(InterruptEventId interrupt_event_id, TaskId task_id);</div>

The [<span class="api">interrupt_event_task_set</span>] API allows a task to configure an interrupt event object by setting the specified task to which the configured signal set is sent when the interrupt event is raised.

/*| doc_configuration |*/
## Interrupt Event Configuration

### `interrupt_events`

The `interrupt_events` configuration is a list of interrupt event configuration objects.

### `name`

This configuration item specifies the interrupt event's name.
Each interrupt event must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with not default.

### `task`

This configuration item specifies the task to which a signal set is sent when the interrupt event is raised.
This configuration item is optional.
If no task is set, raising the interrupt event causes a fatal error.
If the system designer does not set the task in the static configuration, it can be set at runtime using the [<span class="api">interrupt_event_task_set</span>] API.

### `sig_set`

This configuration item specifies the signal set that is sent to the interrupt event's associated task.
A signal set is a list of one or more specified signal labels.
This configuration item is optional and defaults to the empty set.

/*| schema |*/
<entry name="interrupteventid_size" type="int" default="8"/>
<entry name="interrupt_events" type="list" default="[]" auto_index_field="idx">
    <entry name="interrupt_event" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>

/*| public_headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
{{/interrupt_events.length}}

/*| public_type_definitions |*/
{{#interrupt_events.length}}
typedef uint{{interrupteventid_size}}_t {{prefix_type}}InterruptEventId;
{{/interrupt_events.length}}

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#interrupt_events}}
#define {{prefix_const}}INTERRUPT_EVENT_ID_{{name|u}} (({{prefix_type}}InterruptEventId) UINT{{interrupteventid_size}}_C({{idx}}))
{{/interrupt_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
[[#task_set]]
{{#interrupt_events.length}}
void {{prefix_func}}interrupt_event_task_set({{prefix_type}}InterruptEventId interrupt_event_id, {{prefix_type}}TaskId task_id);
{{/interrupt_events.length}}
[[/task_set]]

/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static {{prefix_type}}TaskId interrupt_event_get_next(void);

/*| state |*/
static bool system_is_idle;

/*| function_like_macros |*/
#define interrupt_event_check() (interrupt_application_event_check() || interrupt_system_event_check())
#define interrupt_system_event_check() [[#timer_process]]timer_pending_ticks_check()[[/timer_process]][[^timer_process]]false[[/timer_process]]

/*| functions |*/
static {{prefix_type}}TaskId
interrupt_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        interrupt_event_process();
[[#timer_process]]
        timer_tick_process();
[[/timer_process]]
        next = sched_get_next();

        if (next == TASK_ID_NONE)
        {
            system_is_idle = true;
            interrupt_event_wait();
        }
        else
        {
            system_is_idle = false;
            break;
        }
    }

    internal_assert_task_valid(next);

    return next;
}

/*| public_functions |*/
[[#task_set]]
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_task_set(const {{prefix_type}}InterruptEventId interrupt_event_id, const {{prefix_type}}TaskId task_id)
{
    api_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INVALID_ID);
    api_assert(task_id < {{tasks.length}}, ERROR_ID_INVALID_ID);
    interrupt_events[interrupt_event_id].task = task_id;
}
{{/interrupt_events.length}}
[[/task_set]]
