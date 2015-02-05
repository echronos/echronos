%title Machine QEMU PowerPC e500 package: user manual
%version 1-draft
%docid QS8J4i

Introduction
-------------

The Machine QEMU PowerPC e500 package provides the following example systems for a QEMU emulated PowerPC e500 machine:

<dl>
  <dt>`hello`</dt>
  <dd>A system that does the standard *Hello, world*.</dt>

  <dt>`acamar-system`</dt>
  <dd>A system demonstrating context switch between two tasks via `yield_to()`.</dt>

  <dt>`gatria-system`</dt>
  <dd>A system demonstrating coordination between two tasks via `yield()`, `unblock()`, and mutexes.</dt>

  <dt>`kraz-system`</dt>
  <dd>A system demonstrating coordination between two tasks via `yield()`, mutexes, and signals.</dt>

  <dt>`acrux-system`</dt>
  <dd>A system demonstrating coordination between two tasks via `yield()` and mutexes, driven by timer interrupt events.</dt>

  <dt>`kochab-system`</dt>
  <dd>A system demonstrating task preemption functionality on the Kochab variant, driven by timer interrupt events.</dt>

  <dt>`kochab-signal-demo`</dt>
  <dd>A system demonstrating signal functionality on the Kochab variant.</dt>

  <dt>`kochab-mutex-demo`</dt>
  <dd>A system demonstrating mutex functionality on the Kochab variant.</dt>

  <dt>`kochab-sem-demo`</dt>
  <dd>A system demonstrating semaphore functionality on the Kochab variant.</dt>
</dl>

It also contains code for the following RTOS example program tailored for the QEMU-emulated PowerPC e500:

<dl>
  <dt>`kochab-test`</dt>
  <dd>An example C program demonstrating task preemption functionality on the Kochab variant, driven by timer interrupt events.</dd>
</dl>

The systems in this package can be run on qemu-system-ppc, provided with standard apt package installations of QEMU, emulating the e500 core:

    qemu-system-ppc -S -nographic -gdb tcp::1234 -M ppce500 -kernel path/to/system

This will wait for a GDB connection.
To obtain a PowerPC-compatible build of GDB:

    * clone git repository `git://sourceware.org/git/binutils-gdb.git`

    * check out a branch from a stable release tag, e.g. `gdb-7.7.1-release`

    * run `./configure --target=powerpc-linux-gnu`

    * run `make`

In a separate window, run GDB:

    powerpc-linux-gnu-gdb path/to/system

To connect to QEMU, set a breakpoint at the debug_println stub, and start the system:

    (gdb) target remote :1234
    (gdb) b debug_println
    (gdb) c

Exit GDB with:

    (gdb) quit
    y

After exiting GDB, the escape sequence to exit qemu-system-ppc is `ctrl-A, X`.


`kochab-system`
===============

This system (running the `kochab-test` program) demonstrates the eChronos Kochab variant's task preemption functionality.
It features two tasks, A and B, where task A is assigned a higher priority than task B.

The test program also configures a fixed-interval timer interrupt to occur periodically, and supplies an interrupt handler function, `tick_irq`, that fulfils three responsibilities:

1. Firstly, it takes platform-specific action to clear the interrupt pending status with the PowerPC e500 hardware.
2. Secondly, it raises an interrupt event (which fulfils its role in the test system in waking up A).
3. Finally, it returns true, to indicate that the handler potentially caused a preemption.

Note that in general, a user-supplied interrupt handler function supplied in the system `.prx` configuration MUST be responsible for clearing the hardware condition that caused its interrupt.
Furthermore, if marked `preempting` in the `.prx`, it MUST return a true boolean value if the handler on that run made an action that has the potential to cause a preemption, such as raising an irq event.

Kochab's expected behaviour is to *preempt* the currently running task if an interrupt handler raises an event that causes a higher-priority task to become runnable.
In other words, the RTOS should immediately context switch to the higher-priority task.
This should occur regardless of whether any lower-priority task is currently running, or blocked.

In the first section, task A sends a number of signals to B that are meant to unblock task B, but A should continue to run because it is of a higher priority than B.

After printing "A now waiting for ticks", A goes into a loop where it waits on a signal set that is only delivered whenever an interrupt event is raised by the timer interrupt event handler.
Whenever it receives this signal, it prints "tick" and sends a signal to wake up B.

With task A waiting on interrupt events, lower-priority task B now comes alive, and goes into a loop where it takes turns busy-waiting (via a loop that calls `signal_poll_set`), and waiting (via `signal_wait_set`) on the signal sent by A.

The `kochab-test` program's expected output is as follows:

    Starting RTOS
    task a: taking lock
    task a: releasing lock
    task a
    unblocking b
    task a
    task a
    task a
    task a
    task a
    unblocking b
    task a
    task a
    task a
    task a
    A now waiting for ticks
    tick
    task b: attempting lock
    task b: got lock
    task b blocking
    tick
    task b unblocked
    tick
    (...)
