/*| doc_header |*/

/*| doc_concepts |*/
## Tasks

The microcontroller is a device which executes an stream of instructions that modifies internal state (registers and memory) and controls external devices.
The challenge for an embedded programmer is work out which instructions should be executed to obtain the desired behaviour.

For systems with simple requirements this can be easily achieved with a single big-loop design.
However as the inherent complexity of requirements increases a single big-loop becomes too complicated to effectively develop, reason about or debug.

The diagram below shows an example of this big-loop design.
As more demands are placed on the system the code in the *logic* part of the code becomes too complicated.

<div style="page-break-inside: avoid"><table><tr>
<td width="33%" class="codebox">void main(void)
{
    for (;;) {
        /*... logic ...*/
    }
}</td></tr></table>
<img src="docs/arrow.png" width="100%"/></div>

There are multiple ways that a system designer could start to address this complexity.
The overarching design principle is separation of concerns, where logic that addresses different aspects of the system are separated in some manner to make the complexity more manageable.
One approach to decomposing a system is to structure the functionality so that rather than a single large loop, there are multiple smaller loops, each performing a cohesive set of actions.

The RTOS implements this abstraction by providing tasks.
Each task in the system executes its own loop, and the RTOS provides the underlying mechanism for switching the microcontroller from one task to another.

The diagram below shows three tasks, A, B and C, each with their own loop.
The logic for each of these should be simpler than the case where this logic is mixed in a single big loop.
The arrows show how each task executes independently on the underlying microcontroller.
The end of each arrow represents the point in time where the RTOS switches the microcontroller from running one task to running a different task.

<div style="page-break-inside: avoid"><table><tr>
<td width="33%" class="codebox">void task_a(void)
{
    for (;;) {
        /*... logic ...*/
    }
}</td>
<td width="33%" class="codebox">void task_b(void)
{
    for (;;) {
        /*... logic ...*/
    }
}</td>
<td width="33%" class="codebox">void task_c(void)
{
    for (;;) {
        /*... logic ...*/
    }
}</td>
</tr></table>
<img src="docs/task_arrows.png" width="100%" /></div>

Each task in the system has a unique name chosen by the system designer.
The name is an ASCII string[^task_names] and should be chosen to describe the functionality of the task.
Within the RTOS tasks are referenced by a [<span class="api">TaskId</span>].
Each task has a unique [<span class="api">TaskId</span>] which is assigned automatically be the RTOS configuration tool.
The RTOS configuration tool generates a symbolic macro of the form <span class="api">TASK_ID_<name></span>.
Application code should use this symbolic macro rather than directly using integer values to refer to tasks.

[^task_names]: There are some additional restrictions on valid names.
See the reference section for more details.

Each task in the system has a defined function that implements the given task's logic.
A task's function must have the type signature void `fn(void)`.
It is an error for a task's function to return.
There are no restrictions on two tasks in the system sharing the same function.

Each task in the system has its own unique stack.
The stack is primarily used by the code executing in the task for holding variables and return addresses during function calls.
It is additionally used by the RTOS to save registers during a task switch.
The size of each stack is chosen by the system design, and should be tailored to each task.
Each task may have a different stack size.

The task that is currently executing on the microcontroller is known as the current task.
To implement multiplexing the RTOS provides a context switch mechanism for changing the current task.
The task's context refers to all the state associated with the task but for which the underlying hardware can only support one copy at a time.
Specifically, the processor only supports a single program counter, stack pointer and register state.
Additionally, on the NEO-Dig platform the context includes the stack-protection registers.
During a context switch, the RTOS saves the current task's state (primarily on the task's stack) and then restore the state for the newly current task.

The RTOS provides an interface, [<span class="api">task_current</span>], that can be used to determine the currently executing task.

An RTOS task can be in one of three primary states[^task_states]: current, runnable, or blocked.
Tasks within a system do no usually operate in isolation;
they interact with other tasks, devices and the external environment.
When interacting with another entity the RTOS provides mechanisms so the task can wait until the other entity is ready, rather than the task needing constantly poll the other entity.
When a task is waiting it moves into the blocked state.
There are a number of RTOS operations that cause a task block, such as waiting for a signal, locking a mutex, or sleeping.
When a task moves to the blocked state, it is no longer current, so the RTOS must choose another runnable task to become the current task.
The blocked task unblocks and becomes runnable when the entity it is waiting on is ready.
For example, when a signal is delivered, a mutex is available, or a sleep duration has completed.

<img src="docs/task_states.png" width="100%" />

[^task_states]: There are some additional secondary states to support debugging.

The RTOS is non-preemptive[^preemptiveness], which means that any context switch is actively caused by code that the current task executes.
Conversely, it means that the RTOS never performs a context switch on its own or without an explicit action from the current task itself.
To put it another way, only tasks are active entities in a system, whereas the RTOS merely provides passive library code invoked by tasks.
To ensure that the system as a whole operates correctly it is important that tasks perform context switches on a regular basis[^context_switch_regularity].
For example, if there are any runnable tasks, it is important that they have the opportunity to become the current task.
When tasks are frequently interacting with other entities this is usually not a problem, as the task regularly block, providing the other runnable tasks with an opportunity to become the current task.
There are cases however when a task may wish to provide a long running operation without blocking.
In these cases a task can perform a yield operation (see also [<span class="api">yield</span>] API).
The yield causes the current task to relinquish the CPU allowing another runnable task to become the current task.
The yield operation can be thought of as a block immediately followed by an unblock.

[^context_switch_regularity]: The exact definition of regular in this context depends on the system timing attributes.

[^preemptiveness]: Interrupt handlers preempt tasks, however tasks do not preempt other tasks.

It is possible for the overall system to arrive in a state where all tasks are in the blocked state[^blocked_state].
In this situation there is no current task and the system enters an idle mode.
When the system is in idle mode interrupt handlers is still processed, however no tasks run.
When the system is in idle mode the RTOS places the hardware in to a low-power state.
Tasks may become runnable again when an interrupt handler unblocks the task.

[^blocked_state]: In a system designed to operate in low-power it is desirable for this to be the case most of the time.

When there is more than one task in the runnable state the RTOS must use the scheduling algorithm to determine the task that becomes current.
The scheduling algorithm is described in the next section.

/*| doc_api |*/
## Tasks

### <span class="api">TaskId</span>

A [<span class="api">TaskId</span>] is used to refer to a specific task.
The underlying type is an unsigned integer of a size large enough to represent all tasks[^task_id_type].
The [<span class="api">TaskId</span>] should generally be treated an an opaque value.
Arithmetic operations should not be used on a [<span class="api">TaskId</span>].
For specialised purposes the programmer can assume that a [<span class="api">TaskId</span>] is in the range 0 thru n - 1, where n is the number of tasks in the system.
For example if a per task data structure is required it is valid to use a fixed size array and index the array by using the [<span class="api">TaskId</span>].
For iterating over such an array it is valid use to increment a [<span class="api">TaskId</span>], however care must be taken to ensure the resulting value is in range.
A [<span class="api">TaskId</span>] can be tested for equality, however other logical operations (e.g: comparison) should not be used.
For all tasks in the system, the configuration tool creates a constant with the name `TASK_ID_<name>` that should be used in preference to raw values.

[^task_id_type]: This is normally a `uint8_t`.

### <span class="api">TASK_ID_ZERO</span>

[<span class="api">TASK_ID_ZERO</span>] has the type [<span class="api">TaskId</span>] and represents task zero in the system.
This can be used in cases where application code wish to iterate over all tasks in the system.

### <span class="api">TASK_ID_MAX</span>

[<span class="api">TASK_ID_MAX</span>] has the type [<span class="api">TaskId</span>] and represents the highest task id in the system.
This can be used in cases where application code wish to iterate over all tasks in the system.

### `TASK_ID_<name>`

`TASK_ID_<name>` has the type [<span class="api">TaskId</span>].
A constant is created for each task in the system.
Note that *name* is the upper-case conversion of the task's name.

/*| doc_configuration |*/
## Task Configuration

### `tasks`

The `tasks` configuration is a list of `task` configuration objects.
The configuration must include at least one task.
See the [Task Configuration] section for details on configuring each task.

### `name`

This configuration item specifies the task's name.
Each task must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with not default.

### `function`

The `function` configuration item specifies the task's function.
It must be the name of a function that is available at link time.
The function should have the type `void function(void)`.
The function should not return.
This is a mandatory configuration item with no default.

### `stack_size`

The `stack_size` configuration item specifies the task's stack size.
The RTOS configuration tool automatically creates the task's stack.
This is a mandatory configuration item with no default.

### `start`

This boolean configuration option determines if a task should automatically start when the RTOS is started.
If the task is not automatically started, it must be started using the [<span class="api">task_start</span>] API.
This is an optional configuration item that defaults to false.
At least one task in the system must be configured with `start` as true.

### `signals`

The task may specify a list of task-specific signal labels.

/*| doc_footer |*/
