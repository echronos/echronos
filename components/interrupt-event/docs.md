/*| doc_header |*/

/*| doc_concepts |*/
## Interrupt Service Routines

By default, the CPU executes code in *normal*, non-interrupted context and all RTOS tasks run in this context.
However, in response to interrupts the CPU transitions into interrupt context and executes a previously registered interrupt service routine (ISR).
When an ISR completes, the CPU returns to the normal context and continues to execute the previously interrupted code path.
The exact details of interrupt handling and how ISRs interact with the normal flow of execution heavily depend on the target platform.

RTOS tasks always execute in the normal context which can be interrupted by a hardware interrupt.
This allows the system to respond to hardware events with low latency.

It is the responsibility of the application to implement ISRs that adequately handle the interrupts that can occur.
The RTOS itself is generally not involved in handling interrupts and leaves interrupt handling to applications.
Depending on the RTOS variant and target platform, the RTOS may or may not provide minimal wrappers that allow ISRs to be plain C functions.

ISRs may only use a very small subset of the RTOS API which consists mainly of raising [Interrupt Events].
The majority of the RTOS API is not accessible to ISRs to prevent inconsistencies from arising in the RTOS and system state.
Therefore, applications are recommended to clearly differentiate between code executed in ISRs and task code.
Sharing code between the two modes of execution bears the risk of the shared code using RTOS APIs not available to ISRs.


### Stack Considerations

ISRs execute on the stack of the current task.
For this reason, each ISR should minimise its stack usage.
As the ISR may interrupt any task, each task must have a large enough stack to accommodate both its own usage and the maximum stack usage of any ISR.
On platforms that support and use nested interrupts, there needs to be sufficient stack space to accommodate a task's state plus all nestable ISRs.

### Data Consistency

As the system may interrupt a task at any time, it is important that the ISR modifies any data structures that it may share with tasks in a safe manner.
For example, if an ISR and a task both have code that increments a value (e.g.: `i++`), it is possible for one of the increments to be lost.
Code such as `i++` is implemented with three separate instructions: read from memory, increment value and write to memory.
If an interrupt happens between the read and write instructions, any changes to the variable that the ISR makes would be lost when the tasks write instruction is executed.

One approach to solve this issue is to use instructions that can modify a memory location in a single operation.

### Disabling and Enabling Interrupts

Another option to ensure data consistency is to disable interrupts when updating shared data structures (e.g.: `interrupt_disable(); i++; interrupt_enable()`).
Tasks may freely disable interrupts and enable interrupts to block ISRs from interrupting tasks.

However, RTOS APIs must always be called with interrupts enabled to ensure that they behave correctly.
Calling an RTOS API with interrupts disabled is an implementation error.

Another side effect of this approach is that it increases the latency between a hardware interrupt occurring and the corresponding ISR being executed.
When and for how long a task disables interrupts thus requires particular care because it can influence the timing of ISR processing and task activation.

The RTOS itself ensures that ISRs execute with a low latency, so the RTOS avoids disabling interrupts whenever possible.
As a result, an interrupt may occur during the execution of an RTOS API function, which means that the RTOS state may not be consistent when an ISR executes.
For this reason the ISRs must not call the regular RTOS APIs.
If an ISR were to call any regular RTOS function, it would corrupt RTOS state and cause incorrect system behaviour.

Of course, it is necessary for ISRs to interact with tasks.
To achieve this, a [Interrupt Events] provide a link between ISRs and tasks.


/*| doc_api |*/
## Interrupt Event API

### <span class="api">InterruptEventId</span>

Instances of this type refer to specific interrupt events.
The underlying type is an unsigned integer of a size large enough to represent all interrupt events[^InterruptEventId_width].

[^InterruptEventId_width]: This is normally a `uint8_t`.

Also refer to the [Platform Interrupt Events API] section.

### `INTERRUPT_EVENT_ID_<name>`

These constants of type [<span class="api">InterruptEventId</span>] are automatically generated at build time for each interrupt event in the system.
Note that `<name>` is the upper-case conversion of the interrupt event's name configured through [`interrupt_events/interrupt_event/name`].
Application should treat the numeric values of these constants as opaque values and use them in preference to raw numeric values to refer to interrupt events.

### <span class="api">interrupt_event_task_set</span>

<div class="codebox">void interrupt_event_task_set(InterruptEventId interrupt_event_id, TaskId task_id);</div>

This function configures at run time, which task is signalled when the specified interrupt event is raised.
In some application scenarios, the static configuration via [`interrupt_events/interrupt_event/task`] is sufficient and does not need to be modified at run time.
In that case, this function does not need to be called.
In other scenarios, the task to signal an interrupt event to may change at run time.
In that case, this function allows the application to update this configuration setting dynamically.

/*| doc_configuration |*/
## Interrupt Event Configuration

### `interrupt_events`

The `interrupt_events` configuration is a list of interrupt event configuration objects.

### `interrupt_events/interrupt_event/name`

This configuration item specifies the interrupt event's name.
Each interrupt event must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

Also refer to the [Variant-Specific Interrupt Event Configuration] section.

/*| doc_footer |*/
