%title PowerPC e500 package: user manual
%version 1-draft
%docid r2BkWF

Introduction
-------------

The PowerPC e500 package provides support for a QEMU emulated PowerPC e500 machine.
It provides the following modules:

<dl>
  <dt>`build`</dt>
  <dd>A build module.</dd>

  <dt>`ctxt-switch`</dt>
  <dd>A module that implements context switching.</dd>

  <dt>`vectable`</dt>
  <dd>A module that generates an appropriate vector table for user configuration of interrupt handlers.</dd>

  <dt>`default-linker`</dt>
  <dd>A module that provides a default linker script.</dd>

  <dt>`interrupts-util`</dt>
  <dd>A module that provides assembly routines for query and manipulation of hardware interrupt state.</dd>

  <dt>`debug`</dt>
  <dd>A module that provides the *low-level debug* interface.</dd>

  <dt>`rtos-acamar`</dt>
  <dd>A variant of eChronos that supports context switching between tasks via `yield_to()`.</dd>

  <dt>`rtos-gatria`</dt>
  <dd>A variant of eChronos that supports round-robin scheduling, `yield()`, `block()`, `unblock()`, and mutexes.</dd>

  <dt>`rtos-kraz`</dt>
  <dd>A variant of eChronos that supports round-robin scheduling, `yield()`, mutexes, and signals.</dd>

  <dt>`rtos-acrux`</dt>
  <dd>A variant of eChronos that supports round-robin scheduling, interrupt events, `yield()`, `block()`, `unblock()`, and mutexes.</dd>

  <dt>`rtos-kochab`</dt>
  <dd>A variant of eChronos that supports priority scheduling, mutexes with priority inheritance, semaphores, signals, and interrupts that cause task preemption and trigger the sending of signals.</dd>
</dl>

Please note that RTOS variants that provide interrupt events only support their use by *noncritical external* interrupts on PowerPC, and will not enable or disable any other types of interrupts.
Projects that define handlers for debug, critical, or machine check interrupts must enable them as needed, and cannot use the `interrupt_event_raise()` API or any other RTOS functionality.


`ppce500/build`
==============

The build module supports the *system build* interface.
It assumes the presence of a GNU PowerPC cross-compiler on PATH with toolchain prefix `powerpc-linux-gnu-`.
This toolchain can be obtained via a standard installation of the apt package `gcc-powerpc-linux-gnu`.

The system must also have an assigned linker-script.
The `default-linker` module may be used to provide a functional linker-script.

`ppce500/debug`
==============

The debug module supports the *low-level debug* interface.
It provides the `rtos_internal_debug_putc` function.

### `rtos_internal_debug_putc`

    void rtos_internal_debug_putc(char c)

This function is not currently implemented, and is only a placeholder stub to assist GDB debugging.
Alternatively, the generic `debug_println` function may be another convenient location to place a breakpoint.

`ppce500/vectable`
=================

The `vectable` module provides a vector table implementation for the PowerPC e500.

It provides support for the user to provide a handler function for any of the following PowerPC e500 interrupts:

* `machine_check`
* `critical_input`
* `watchdog_timer`
* `debug`
* `system_call`
* `data_storage`
* `instruction_storage`
* `external_input`
* `alignment`
* `program`
* `floating_point`
* `aux_processor`
* `decrementer`
* `fixed_interval_timer`
* `data_tlb_error`
* `instruction_tlb_error`
* `eis_spe_apu`
* `eis_fp_data`
* `eis_fp_round`
* `eis_perf_monitor`

A user can configure the handler function for a system by setting the `handler` tag for the interrupt to the C identifier of the function in the system's `.prx` file, for example:

    <module name="ppce500.vectable">
      <fixed_interval_timer>
        <handler>tick_irq</handler>
      </fixed_interval_timer>
    </module>

In general, a user-supplied interrupt handler function like `tick_irq` in the example above MUST be responsible for clearing the hardware condition that caused its interrupt.

When configuring a system using an RTOS variant that provides preemption support (such as Kochab), the vectable module must must have the `preemption` tag set to `true` in the `.prx` file:

    <module name="ppce500.vectable">
      <preemption>true</preemption>
    </module>

(Note: when `preemption` is enabled, the system call vector is no longer available for configuration, as the RTOS makes use for this vector internally to implement preemption support.)

On preemption-supporting RTOS variants, interrupts of the *non-critical* class may be configured as being `preempting`, by setting their `preempting` tag to `true` in the `.prx` file:

    <module name="ppce500.vectable">
      <preemption>true</preemption>
      <fixed_interval_timer>
        <handler>tick_irq</handler>
        <preempting>true</preempting>
      </fixed_interval_timer>
    </module>

The user should configure an interrupt as `preempting` when its interrupt handler has the potential to cause task preemption.
Handler functions marked `preempting` MUST return a boolean value that is true if the handler has just made an action with the potential to cause a preemption, such as raising an interrupt event.
These requirements must be adhered to for the RTOS to be able to enforce its scheduling policies.
