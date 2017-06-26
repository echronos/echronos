/*| provides |*/
mpu-armv7m

/*| requires |*/
rtos
task

/*| doc_header |*/
/*| doc_concepts |*/
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
