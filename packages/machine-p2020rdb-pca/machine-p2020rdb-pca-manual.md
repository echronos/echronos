<!---
eChronos Real-Time Operating System
Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, version 3, provided that no right, title
or interest in or to any trade mark, service mark, logo or trade name
of NICTA or its licensors is granted.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@TAG(NICTA_DOC_AGPL)
  -->

%title Machine P2020RDB-PCA package: user manual
%version 1-draft
%docid mllu0K

Introduction
-------------

The Machine P2020RDB-PCA package provides the following example systems for the Freescale P2020RDB-PCA:

<dl>
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

  <dt>`kochab-timer-demo`</dt>
  <dd>A system demonstrating timer API functionality on the Kochab variant.</dd>

  <dt>`kochab-sched-demo`</dt>
  <dd>A system demonstrating scheduling behavior on the Kochab variant.</dd>

  <dt>`phact-signal-demo`</dt>
  <dd>A system demonstrating signal functionality on the Phact variant.</dd>

  <dt>`phact-mutex-demo`</dt>
  <dd>A system demonstrating mutex functionality on the Phact variant.</dd>

  <dt>`phact-sem-demo`</dt>
  <dd>A system demonstrating semaphore functionality on the Phact variant.</dd>

  <dt>`phact-timer-demo`</dt>
  <dd>A system demonstrating timer API functionality on the Phact variant.</dd>

  <dt>`phact-sched-demo`</dt>
  <dd>A system demonstrating scheduling behavior on the Phact variant.</dd>
</dl>

The systems provided in this package build to ELF format, and can be booted on the Freescale P2020RDB-PCA board using its stock U-Boot image's `bootelf` command.
System binaries may be loaded into the board's RAM using U-Boot's `tftpboot` command or some other method.
Note that you must manually disable interrupts in the bootloader before booting each RTOS image:

    => tftpboot
    => interrupts off
    => bootelf
