/*| provides |*/
context-switch-preempt-armv7m
/*| requires |*/
preempt
/*| doc_header |*/
<!-- %architecture ARMv7m -->
/*| doc_concepts |*/
## Internal RTOS Interrupts

Internally the RTOS makes use of the `PendSV` and `SVCall` interrupts to facilitate context switching on pre-emptive OS variants.
The majority of ARM devices implement at least 4 bits of *interrupt priority resolution*, which allows for 16 different levels of interrupt priority.
By default, the RTOS is configured to operate with correct priority settings on any device with 4 or more such bits.
However, ARM devices with less than 4 bits of priority resolution are not uncommon.
As such, it's possible to configure these priorities manually using the `svcall_priority` and `pendsv_priority` configuration items.

### Calculating Internal Interrupt Priorities

An ARM core that has *n* bits of interrupt priority resolution *only* uses the *n* most significant bits of `svcall_priority` and `pendsv_priority` when resolving interrupt priorities.
Additionally, it is imperative for the internal context switching mechanism that `svcall_priority` have a numerically distinct, and lower value than `pendsv_priority`.

Usually, these priorities are chosen to be as (numerically) high as possible - for example with 4 bits of priority, appropriate choices for the most significant 4 bits of priority are `1110` and `1111` for `svc` and `pendsv` respectively.
Noting that the priority register has a size of 8 bits, appropriate choices for final priorities are `1110 << 4 = 11100000 = 224` and `1111 << 4 = 11110000 = 240` (note that these are the default values for this RTOS variant).
In general, for an ARM core with *n* bits of priority resolution, `svcall_priority` is set to `(2^n-2) << (8-n)` and `pendsv_priority` is set to `(2^n-1) << (8-n)`.

/*| doc_api |*/
/*| doc_configuration |*/
## Internal RTOS Interrupt Priority Configuration

### `svcall_priority`

This configuration item is an integer with a default value of 224, a value appropriate in systems with 4 or more bits of interrupt priority resolution.
See section [Internal RTOS Interrupts] for more information.

### `pendsv_priority`

This configuration item is an integer with a default value of 240, a value appropriate in systems with 4 or more bits of interrupt priority resolution.
See section [Internal RTOS Interrupts] for more information.

/*| doc_footer |*/
