<!---
eChronos Real-Time Operating System
Copyright (C) 2017  National ICT Australia Limited (NICTA), ABN 62 102 206 173.

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
# Quick-start for ARMv7m

## Prerequisites

Obtain the source code with the command

    git clone --depth=1 https://github.com/echronos/echronos.git

To obtain the `arm-none-eabi` GNU toolchain and `gdb-arm-none-eabi` for building and debugging the RTOS for ARMv7-M, run:

    sudo apt-get install gcc-arm-none-eabi gdb-arm-none-eabi

The packages will have slightly different names across different linux distributions and versions.

Note: Older versions of Ubuntu have a known bug with ARM gdb package installation (see [here](https://bugs.launchpad.net/ubuntu/+source/gdb-arm-none-eabi/+bug/1267680)).
If you are unable to install it due to a conflict, try adding a dpkg diversion for the gdb man pages first:

    sudo dpkg-divert --package gdb --divert /usr/share/man/man1/arm-none-eabi-gdbserver.1.gz --rename /usr/share/man/man1/gdbserver.1.gz
    sudo dpkg-divert --package gdb --divert /usr/share/man/man1/arm-none-eabi-gdb.1.gz --rename /usr/share/man/man1/gdb.1.gz

And then retry the above installation command for `gdb-arm-none-eabi`.

To obtain `echronos-qemu-system-arm` for emulating ARM systems, read the README.md for [our QEMU fork](https://github.com/echronos/qemu/) (make sure to use the `echronos-qemu` branch).

On most linux distributions, it will be simplest to use the binaries included in the QEMU fork repository - see 'Using the binaries' section of the QEMU README.md.

## Running an example system

Build and run an example system for the RTOS variant *Gatria* on QEMU-emulated ARMv7-M (STM32):

    cd echronos
    prj/app/prj.py build machine-stm32f4-discovery.example.gatria-system

    # Run the generated system in qemu (press `ctrl-c` to close QEMU after it is finished)
    echronos-qemu-system-arm -mcu STM32F407VG -semihosting -S -s --kernel `pwd`/out/machine-stm32f4-discovery/example/gatria-system/system

    # To connect and view debug output run gdb in another shell prompt
    arm-none-eabi-gdb -ex "target remote :1234" out/machine-stm32f4-discovery/example/gatria-system/system
    (gdb) b fn_a
    Breakpoint 1 at 0x800065c: file packages/rtos-example/gatria-test.c, line 41.
    (gdb) c
    Continuing.

    Breakpoint 1, fn_a () at packages/rtos-example/gatria-test.c:41
    41	{
    (gdb) n
    43	    rtos_unblock(0);
    (gdb) n
    44	    rtos_unblock(1);
    (gdb) c
    Continuing.
    task a -- lock
    task b -- try lock
    task a -- unlock
    ...
