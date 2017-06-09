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
# Quick-start for PowerPC e500 Targets

## Prerequisites

Obtain the source code with the command

    git clone --depth=1 https://github.com/echronos/echronos.git

To obtain the `powerpc-linux-gnu` GNU toolchain for building the RTOS for PowerPC e500 on Ubuntu Linux systems, run:

    sudo apt-get install gcc-powerpc-linux-gnu

To obtain the `qemu-system-ppc` emulator for running the [`machine-qemu-ppce500`](../packages/machine-qemu-ppce500) systems on Ubuntu Linux systems, run:

    sudo apt-get install qemu-system-ppc

To obtain, build, and install `powerpc-linux-gdb` for debugging PowerPC e500 systems, run:

    wget https://ftp.gnu.org/gnu/gdb/gdb-7.12.tar.xz
    tar xaf gdb-7.12.tar.xz
    cd gdb-7.12
    ./configure --target=powerpc-linux --prefix=/usr/
    make -s
    sudo make -s install

## Running an example system

Build and run an example system for the RTOS variant *Kochab* on QEMU-emulated PowerPC e500:

    cd echronos
    prj/app/prj.py build machine-qemu-ppce500.example.kochab-system

    # Run the generated system in qemu (press `ctrl-a` then 'x' to close QEMU after you are finished)
    qemu-system-ppc -M ppce500 -S -s -nographic -kernel out/machine-qemu-ppce500/example/kochab-system/system

    # To connect and view debug output run gdb in another shell prompt
    # Note: The Kochab example will end with task b cycling forever between "blocked" and "unblocked"

    powerpc-linux-gdb -ex "target remote :1234" -ex "b debug_print" out/machine-qemu-ppce500/example/kochab-system/system
    (gdb) c
    ...
    Breakpoint 1, debug_print (msg=0x33b4 "tick")
    (gdb) c
    Breakpoint 1, debug_print (msg=0x33e8 "task b blocking")
    (gdb) c
    Breakpoint 1, debug_print (msg=0x33b4 "tick")
    (gdb) c
    Breakpoint 1, debug_print (msg=0x33f8 "task b unblocked")
    (gdb) quit
