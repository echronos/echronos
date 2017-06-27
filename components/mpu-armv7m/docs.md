/*| provides |*/
mpu-armv7m

/*| requires |*/
rtos
task

/*| doc_header |*/
/*| doc_concepts |*/
## Memory Protection

The RTOS supports hardware memory protection by using the on-chip Memory Protection Unit (MPU).
Using memory protection can protect systems from many different types of errors, from simple programming mistakes to hardware failures.
The RTOS's implementation of memory protection provides:

- *Memory Isolation*: tasks may only access memory they have been explicitly granted permission to use.

- *Privileged/User modes*: only the RTOS kernel should perform some critical operations and thus runs in privileged mode.

- *Determinism*: any violation of protection policies will result in a predictable execution path after the violation.

### Protection Model

If memory protection is enabled, and no extra configuration information is supplied:

- Tasks will *only* be able to read/write to their stack.

- All tasks will, in addition, be granted *read-only access to the code segment*.
Note that this means tasks are not prevented from running code that does not belong to them[^mpu_code_segment].

- Tasks *may still make RTOS 'system calls'*, which will now result in a switch to privileged mode, execution of the call, and then a return to the next schedulable task in usermode.

- Any *protection faults will cause the `fatal_error` handler to be called* with the protection fault error code.

[^mpu_code_segment]: Protecting the data and not the code is standard memory protection practice with such limited devices - it is a functionality tradeoff.

### Protection Domains, Data & Tasks

A key concept related to memory protection is how tasks are granted permission to access memory regions.
The RTOS implements 'Protection Domains' as a way of bookkeeping memory regions.
A *protection domain* can contain one or more pieces of data (this is known as a 'symbol' domain) *or* encompass an address range, like a peripheral (this is known as an 'address' domain).
Tasks can be given *different permissions* to these domains, depending on what they require.
This facilitates decoupling of logical functionality and memory. For example:

<img src="docs/domain_abstraction.png"/>

In the above diagram, Domain A is an 'address' domain that encompasses a GPIO peripheral, Domain B is a 'symbol' domain that contains some data, and Domain C is another 'address' domain encompassing an address range for an on-chip ROM.
Task A is the only task that may write to Domain B, and thus can dictate how the 'STATE' data appears to Task B and Task C, with a guarantee that the data will not be modified by Task B or Task C.
Task B is the only task that has access to Domain C, and so access to any memory in Domain C by Task A or Task C will cause a protection fault.

Note that if a task has no domain associations at all, it will only be able to access it's own stack and make RTOS calls.

When creating a system, the RTOS configuration mechanism is used to create and assign protection domains (See [Memory Protection Configuration] for more information).

### The ARMv7m Memory Protection Unit

Note that some devices in the ARMv7m family do not have an MPU, be sure to check the vendor documentation before using this feature.
Most ARMv7m MPUs have the same set of capabilities:

- *8 protection regions* (16 on some Cortex-M7 processors, but this is rare)
- Each region has *Readable / Writeable / Executeable flags*
- Each region has a *base address and region size*

A 'protection region' is essentially a partition of the processor's address space that enforces an access restriction.
Had we set up some basic protection regions, the address space might look like:

<img src="docs/mpu_hardware.png"/>

It is obvious that since there is a limitation on the number of protection regions, this must place some limitation on the RTOS.
Since the RTOS will always need protection regions active during task execution to indicate both:

- The task stack *and*
- The system code section

This leaves `8 - 2 = 6` regions for general-purpose use.
In practice, this means that tasks may only have a maximum of *6* associated domains on this architecture.
That is, any single task may only be granted access permissions to a maximum of *6* protection domains.

Active protection regions are changed at runtime depending on which task is currently scheduled, and which protection domains the task has access to. For example:

<img src="docs/mpu_visualization.png"/>

With task B currently running, the active protection regions will be those corresponding to the system code segment, the stack of task b, the 'command domain' and the 'uart domain'. In the event that a new task is scheduled, the active protection regions will be changed to suit the new task in accordance with the RTOS configuration.

/*| doc_api |*/
/*| doc_configuration |*/
## Memory Protection Configuration

### `memory_protection`

This configuration item is a boolean with a default of false.
When true, memory protection capabilities of the RTOS will be enabled.
The protection policies enforced depend on the rest of the memory protection configuration items.
If no other configuration options are set, every task will only have access to it's own stack.
Memory protected systems must be compiled with a special set of flags, so ensure that that the correct build scripts are used by the system to avoid strange error messages when memory protection is enabled.

### `verbose_protection_faults`

This configuration item is a boolean with a default of false.
When true, instead of a protection fault directly resulting in calling the RTOS `fatal_error` function, the RTOS will first emit debugging text indicating the fault address and status.
The text will be emitted using the RTOS's default `debug_print` functionality.

### `skip_faulting_instructions`

This configuration item is a boolean with a default of false.
When true, instead of a protection fault calling the RTOS `fatal_error` function, any instructions that cause a protection fault will be skipped.
It is intended for use in demo modules and for debugging.
This functionality is not recommended for use in 'real' systems, because it can easily cause unintended infinite loops and other undesired behaviour.

### `protection_domains`

This configuration item is a list of `protection_domain` configuration objects
A protection domain denotes a named area of memory that can be associated with tasks.
Tasks define the permissions they have to protection domains in their task configuration.

### `protection_domains/protection_domain/name`

This configuration item specifies the protection domain's name.
Each domain must have a unique name, and be of an identifier type.
This is a mandatory configuration item with no default.

### `protection_domains/protection_domain/symbols`

This configuration item is a list of `symbol` configuration objects.
A protection domain with this field is known as a 'symbol' domain.
It collects C symbols from the project into a named area of memory.
Either `symbols` or `base_address` needs to be specified for a protection domain.

### `protection_domains/protection_domain/symbols/symbol`

This configuration item denotes either a C identifier, or an object file that corresponds to a source file after the build process.
For example, if we had an implementation file, `foobar.c`, which contained:

    int foo_int;
    int bar_int;

If we desired to include all global data elements from the file, we could use `foobar.o` as our symbol.
If we instead desired to include only `bar_int`, we would indicate `bar_int` as our symbol.

### `protection_domains/protection_domain/base_address`

This configuration item denotes the start of a memory address range.
A protection domain with this field is known as an 'address' domain.
It creates a named area of memory from the supplied `base_address` up to `base_address+domain_size`.
This is commonly used (for example) when giving tasks access to hardware peripherals.
Either `symbols` or `base_address` needs to be specified for a protection domain.

### `protection_domains/protection_domain/domain_size`

This configuration item denotes the size of the protection domain in bytes.
For a 'symbol' domain, this indicates how much memory the protection domain will use, irrespective of whether it is full of symbols or not.
If all the symbols provided do not fit into the indicated `domain_size`, the system will fail to build.
For an 'address' domain, this indicates the size of the address range corresponding to the domain.
This is a mandatory configuration item with no default.

### `tasks/task/associated_domains`

This configuration item is a list of `domain` configuration objects.
Each `domain` entry defines permissions that tasks have to a specific protection domain.
A task will not have read or write access to any protection domains unless it is associated with them.
This is not a mandatory configuration item, with a default of no associated domains.

### `tasks/task/associated_domains/domain/name`

This configuration item specifies the protection domain that this entry associates the task with.
This property must have the same value as the corresponding `protection_domains/protection_domain/name`
This is a mandatory configuration item with no default.
A domain association with no properties except the domain name will result in read-only access.

### `tasks/task/associated_domains/domain/readable`

This configuration item is a boolean with a default of true.
When true, the task may read from the indicated protection domain without causing a protection fault.

### `tasks/task/associated_domains/domain/writeable`

This configuration item is a boolean with a default of false.
When true, the task may write to the indicated protection domain without causing a protection fault.
Note that on most architectures, writeable permissions without readable permissions are not valid.

### `tasks/task/associated_domains/domain/executable`

This configuration item is a boolean with a default of false.
When true, the task may execute code in the indicated protection domain without causing a protection fault.
This is commonly used when (for example) the processor requires execution from a ROM to perform certain functions.
Note that on most architectures, executable permissions without readable permissions are not valid.

/*| doc_footer |*/
