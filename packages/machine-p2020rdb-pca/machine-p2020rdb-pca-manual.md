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

  <dt>`kochab-interrupt-demux-example`</dt>
  <dd>A (Kochab) system demonstrating demultiplexing of external interrupts using the P2020 PIC, and their delivery to particular tasks using eChronos' `interrupt_event_raise()` API.</dd>

  <dt>`kochab-interrupt-buffering-example`</dt>
  <dd>A (Kochab) system demonstrating buffering of data between interrupt handler and task, with access to the data synchronized via some platform-specific method, as well as interrupt-driven receipt/transmission of data via the P2020 DUART.</dd>

  <dt>`kochab-task-sync-example`</dt>
  <dd>A (Kochab) system demonstrating transfer of data between two tasks, with access to the data synchronized using some eChronos API, as well as interrupt-driven receipt/transmission of data via the P2020 DUART.</dd>

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

  <dt>`phact-interrupt-demux-example`</dt>
  <dd>A (Phact) system demonstrating demultiplexing of external interrupts using the P2020 PIC, and their delivery to particular tasks using eChronos' interrupt_event_raise API.</dd>

  <dt>`phact-interrupt-buffering-example`</dt>
  <dd>A (Phact) system demonstrating buffering of data between interrupt handler and task, with access to the data synchronized via some platform-specific method, as well as interrupt-driven receipt/transmission of data via the P2020 DUART.</dd>

  <dt>`phact-task-sync-example`</dt>
  <dd>A (Phact) system demonstrating transfer of data between two tasks, with access to the data synchronized using some eChronos API, as well as interrupt-driven receipt/transmission of data via the P2020 DUART.</dd>
</dl>

The systems provided in this package build to ELF format, and can be booted on the Freescale P2020RDB-PCA board using its stock U-Boot image's `bootelf` command.
System binaries may be loaded into the board's RAM using U-Boot's `tftpboot` command or some other method.
Note that you must manually disable interrupts in the bootloader before booting each RTOS image:

    => tftpboot
    => interrupts off
    => bootelf

This package also contains the C code for the following P2020-specific example programs:

<dl>
  <dt>`interrupt-demux-example`</dt>
  <dd>An example C program demonstrating demultiplexing of external interrupts using the P2020 PIC, and their delivery to particular tasks using eChronos' `interrupt_event_raise()` API.</dd>

  <dt>`interrupt-buffering-example`</dt>
  <dd>An example C implementation of an interrupt handler demonstrating receipt of data via the P2020 DUART, and buffering of that data from interrupt handler to task, with access to the data synchronized via some platform-specific method.</dd>

  <dt>`task-echo-example`</dt>
  <dd>An example C implementation of a task demonstrating receipt of data from a buffering interrupt handler, and interrupt-driven transmission of data via the P2020 DUART.</dd>
</dl>

  <dt>`task-sync-example`</dt>
  <dd>An example C implementation of two tasks demonstrating transfer of data (received from a buffering interrupt handler) from one task to another, with access to the data synchronized using some eChronos API, as well as interrupt-driven transmission of data via the P2020 DUART.</dd>
</dl>

To support code reuse across the above examples, this package also contains some P2020-specific "library" modules:

<dl>
  <dt>`p2020-util`</dt>
  <dd>Implements `debug_putc()`, and allows the P2020's Configuration, Control, and Status Registers Base Address Register (CCSRBAR) value, needed to derive the base register addresses for other P2020 devices, to be specified by a single .prx configuration entry.</dd>

  <dt>`p2020-duart`</dt>
  <dd>Implements a limited driver interface for P2020's Dual Universal Asynchronous Receiver/Transmitters (DUARTs).</dd>

  <dt>`p2020-pic`</dt>
  <dd>Implements a limited driver interface for P2020's Programmable Interrupt Controller (PIC).</dd>
</dl>


`interrupt-demux-example`
=========================

This example system serves to demonstrate use of the P2020 PIC, as well as interrupt delivery to distinct tasks using eChronos' `interrupt_event_raise()` API.

The program configures the following external interrupt sources, and handles them with the `exti_interrupt` function:
- P2020 DUART1 RX (on the P2020RDB-PCA, this is the RJ45 port labelled "UART0")
- P2020 DUART2 RX (on the P2020RDB-PCA, this is the RJ45 port labelled "UART1")
- All 8 of the P2020 PIC' global timers, configured to occur at periods that are multiples of each other

The program also configures the PowerPC e500 CPU's fixed interval timer, and handles it separately with the `tick_interrupt` function.

Each distinct interrupt source will trigger an interrupt delivery to a distinct task running in the system.

DUART1 TX is used for (busy-waiting) debug prints, and the program relies on these prints for demonstration purposes.
Each task in the program will print its name whenever it receives a signal as a result of an interrupt event delivery.
Furthermore, the interrupt handlers also print some information to give clues as to how the demuxing works.

DUART2 TX is not used.


`interrupt-buffering-example`
=============================

This example system functions as an echo server for characters typed into P2020's DUART2 device, and all use of DUART2 is interrupt-driven.
The interrupt handler (implemented by `interrupt-buffering-example.c`) first buffers the data from DUART2's RX FIFO to a task (implemented by `task-echo-example.c`), which then transmits the data via DUART2's TX FIFO.

The system serves to demonstrate one possible way to synchronize the buffering of data from interrupt handler to task.
Generally, this must rely on some platform-specific method, because interrupts are a platform-specific feature.
In this implementation, we choose to synchronize the interrupt handler and task's concurrent access to the buffer by having the task disable interrupts using the `wrteei` instruction while it accesses the buffer.

DUART1 is used only for (busy-waiting) debug prints for unexpected error cases.


`task-sync-example`
===================

This example system functions as a kind of "reverse-echo" server for characters typed into P2020's DUART2 device.
It extends the `interrupt-buffering-example` system by replacing `task-echo-example.c` with `task-sync-example.c`, which has a 1st task (A) wait for a "chunk" of data to arrive from the DUART2's RX FIFO before transferring it to a 2nd task (B), which then transmits the chunk via DUART2's TX FIFO with its characters reversed.
Each chunk of data is delimited either by a carriage-return character, or by the MSG_SIZE, whichever comes first.
Messages of zero size (i.e. consecutive carriage-return characters) are ignored.

The system serves to demonstrate one possible way to synchronize the transfer of data between two tasks.
In eChronos, this is possible using eChronos' synchronization primitives such as signals, mutexes, or semaphores.
In this implementation, we choose to synchronize the two tasks' concurrent access to the shared data structure by having task A send RTOS_SIGNAL_ID_RX to task B when a chunk of data is ready for transmission, and task B send RTOS_SIGNAL_ID_TX to task A when it is ready to transmit another chunk.

DUART1 is used only for (busy-waiting) debug prints for unexpected error cases.
