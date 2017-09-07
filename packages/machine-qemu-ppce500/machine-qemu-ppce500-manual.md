<!--
     eChronos Real-Time Operating System
     Copyright (c) 2017, Commonwealth Scientific and Industrial Research
     Organisation (CSIRO) ABN 41 687 119 230.

     All rights reserved. CSIRO is willing to grant you a licence to the eChronos
     real-time operating system under the terms of the CSIRO_BSD_MIT license. See
     the file "LICENSE_CSIRO_BSD_MIT.txt" for details.

     @TAG(CSIRO_BSD_MIT)
-->
%title Machine QEMU PowerPC e500 package: user manual
%version 1-draft
%docid QS8J4i

Introduction
-------------

The Machine QEMU PowerPC e500 package provides the following example systems for a QEMU emulated PowerPC e500 machine:

<dl>
  <dt>`hello`</dt>
  <dd>A system that does the standard *Hello, world*.</dd>

  <dt>`acamar-system`</dt>
  <dd>A system demonstrating context switch between two tasks via `yield_to()`.</dd>

  <dt>`gatria-system`</dt>
  <dd>A system demonstrating coordination between two tasks via `yield()`, `unblock()`, and mutexes.</dd>

  <dt>`kraz-system`</dt>
  <dd>A system demonstrating coordination between two tasks via `yield()`, mutexes, and signals.</dd>

  <dt>`acrux-system`</dt>
  <dd>A system demonstrating coordination between two tasks via `yield()` and mutexes, driven by timer interrupt events.</dd>

  <dt>`kochab-system`</dt>
  <dd>A system demonstrating task preemption functionality on the Kochab variant, driven by timer interrupt events.</dd>

  <dt>`kochab-signal-demo`</dt>
  <dd>A system demonstrating signal functionality on the Kochab variant.</dd>

  <dt>`kochab-mutex-demo`</dt>
  <dd>A system demonstrating mutex functionality on the Kochab variant.</dd>

  <dt>`kochab-sem-demo`</dt>
  <dd>A system demonstrating semaphore functionality on the Kochab variant.</dd>
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

    powerpc-linux-gdb path/to/system

To connect to QEMU, set a breakpoint at the debug_println stub, and start the system:

    (gdb) target remote :1234
    (gdb) b debug_println
    (gdb) c

Exit GDB with:

    (gdb) quit
    y

After exiting GDB, the escape sequence to exit qemu-system-ppc is `ctrl-A, X`.
