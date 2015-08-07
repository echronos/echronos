<!---
eChronos Real-Time Operating System
Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, version 3, provided that these additional
terms apply under section 7:

  No right, title or interest in or to any trade mark, service mark, logo
  or trade name of of National ICT Australia Limited, ABN 62 102 206 173
  ("NICTA") or its licensors is granted. Modified versions of the Program
  must be plainly marked as such, and must not be distributed using
  "eChronos" as a trade mark or product name, or misrepresented as being
  the original Program.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@TAG(NICTA_DOC_AGPL)
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

    powerpc-linux-gnu-gdb path/to/system

To connect to QEMU, set a breakpoint at the debug_println stub, and start the system:

    (gdb) target remote :1234
    (gdb) b debug_println
    (gdb) c

Exit GDB with:

    (gdb) quit
    y

After exiting GDB, the escape sequence to exit qemu-system-ppc is `ctrl-A, X`.
