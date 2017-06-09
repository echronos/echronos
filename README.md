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
Core repository:
[![Core Build Status on Linux](https://travis-ci.org/echronos/echronos.svg?branch=master)](https://travis-ci.org/echronos/echronos)
[![Core Build Status on Windows](https://ci.appveyor.com/api/projects/status/u0l9tcx3r8x9fwj0/branch/master?svg=true)](https://ci.appveyor.com/project/stefangotz/echronos/branch/master)

Client repository:
[![Client Build Status on Linux](https://travis-ci.org/echronos/test-client-repo.svg?branch=master)](https://travis-ci.org/echronos/test-client-repo)
[![Client Build Status on Windows](https://ci.appveyor.com/api/projects/status/wbyntsf0a5crcl62/branch/master?svg=true)](https://ci.appveyor.com/project/stefangotz/test-client-repo/branch/master)

---

# Yes, We Are Open for Business

If you have any questions, send us an e-mail to echronos@trustworthy.systems or tweet at us [@echronosrtos](https://twitter.com/echronosrtos).

If there is something in the project that you think is worth improving, please create a [github issue](https://github.com/echronos/echronos/issues).

Of course, we are also keen on your changes and contributions if you have any - [here is a primer](CONTRIBUTING.md).


# Overview

The eChronos RTOS is a real-time operating system (RTOS) originally developed by NICTA and Breakaway Consulting Pty. Ltd.

It is intended for tightly resource-constrained devices without memory protection.
To this end, the RTOS code base is designed to be highly modular and configurable on multiple levels, so that only the minimal amount of code necessary is ever compiled into a given system image.

Available implementations currently target ARM Cortex-M4 and PowerPC e500.
The RTOS also runs on POSIX platforms (e.g., Linux, MacOS-X, Windows with cygwin or MinGW) for rapid prototyping.


# Quick-start for Linux and Windows

Get a first impression of how the RTOS is being used without delving into the details of any embedded hardware platform.
This section covers how to build and run a simple application on top of the RTOS as a regular Linux or Windows process.

## Download

Download the [latest _posix_ release](https://github.com/echronos/echronos/releases) and unpack it in a directory of your choice.

The project makes frequent releases, based on improvements flowing into the master branch of the code repository.
You'll notice that releases are not just snapshots of the code repository.
Rather, they provide just the bits and pieces needed for individual target platforms, with some of the code and tools already pre-built.

The _posix_ release is aimed at running the RTOS on top of any POSIX host operating system, such as Linux or Windows (with Cygwin or MinGW).

## Prerequisites

You need the following tools installed and ready to go:

- Python 3
- GCC compiler + GNU binutils

On Linux, use your package manager to install these tools.
On Windows, obtain and install Python 3 from [python.org](https://www.python.org) and install either [Cygwin](https://cygwin.com) or [MinGW](http://mingw.org) including the GCC compiler.

## Build

The following commands build a simple version of the eChronos RTOS together with a small example application:

    cd eChronos-posix-VERSION
    ./bin/prj.sh build posix.acamar

On Windows, run `prj\prj` instead of `./prj/prj.sh`.
This produces the binary `out/posix/acamar/system` (`system.exe` on Windows).

## Run

Run the sample system with the command `./out/posix/acamar/system`.
It prints `task a` and `task b` to the screen until you stop it by pressing `ctrl-c`.

If you have GDB installed, you can also run this RTOS system in a debugger.
Start GDB with `gdb out/posix/acamar/system` and

- set a break point with `b debug_print`
- start the system with `run`
- gdb now stops the system just before it prints `task a` or `task b`, allowing you to inspect the system state or to continue with `continue`

## Under the Hood

### The `prj` Tool

To give you an idea of what goes on when building an running an RTOS system as above, here is a quick overview of what happens under the hood.

The `prj` tool is the build tool of the RTOS.
Its primary purpose is to
1. find and load the system configuration,
1. generate RTOS code specifically for the system configuration,
1. and build the RTOS and application code into a single system binary.

The command `./prj/prj.sh build posix.acamar` makes `prj` first search for a system configuration file with a `.prx` file name extension.
Think of PRX files as make files.
`prj` finds that system configuration file at [`share/packages/posix/acamar.prx`](packages/posix/acamar.prx), based on the search paths set up in [`project.prj`](prj/release_files/project.prj).

[`acamar.prx`](packages/posix/acamar.prx) lists all the files that go into building this system plus some configuration information.
For example, this system is configured with two tasks, _a_ and _b_, that have given entry point functions and stack sizes:

    <task>
        <name>a</name>
        <function>fn_a</function>
        <stack_size>8192</stack_size>
    </task>
    <task>
        <name>b</name>
        <function>fn_b</function>
        <stack_size>8192</stack_size>
    </task>

In the second step, `prj` uses this configuration information to generate a custom copy of the RTOS source code.
This makes the RTOS itself as small as possible to leave more resources for the application.

The third step is to invoke the compiler and linker to build all the source code files listed in the system configuration into a binary.
`prj` invokes the POSIX-specific [build module](packages/posix/build.py) to achieve that.

### The Sample Application

The file [`share/packages/rtos-example/acamar-test.c`](packages/rtos-example/acamar-test.c) contains the main application code of the example system (the PRX file refers to it as `rtos-example.acamar-test`).
This file implements two tasks that perpetually print their name and yield to each other.

You will notice that this file also contains the standard `main()` function found in all C programs.
If necessary, it could, for example, initialize some hardware before starting the RTOS, which in turn starts the two tasks.

### What is _Acamar_?

The eChronos project is not a single RTOS, but provides a family of RTOS variants with different feature sets.
Acamar is the name of the smallest one, but the POSIX release comes with a number of other, more powerful variants.
Those provide "proper" RTOS features, such as mutexes, interrupts, and timers.
The [Variants and Components](docs/software_model.md#variants-and-components) section has more information on this topic.

# Documentation

Basic RTOS concepts and usage are documented in README file and the [`docs`](docs) directory.

More detailed documentation for the `x.py` tool can be found inside [`x.py`](x.py) itself.
More detailed documentation for the `prj` tool can be found in [`prj/manual/prj-user-manual`](prj/manual/prj-user-manual).

Pregenerated RTOS API manuals can be found on the [eChronos GitHub wiki](https://github.com/echronos/echronos/wiki) or you can [build them yourself](docs/common_development_tasks.md#building-user-documentation).

# Where to from here?

The rest of this README covers ARM and PowerPC quick-starts as well as all basic RTOS concepts and how to make use of them.
It makes use of the full [source code repository](https://github.com/echronos/echronos/), not just a release as the Quick-start Guide did above.

- [quick-start ARMv7m](docs/quickstart_armv7m.md)
- [quick-start PowerPC](docs/quickstart_powerpc.md)
- [software model](docs/software_model.md)
- [common development tasks](docs/common_development_tasks.md)
- [repository structure](docs/repository_structure.md)
- [task management](docs/task_management.md)
- [project conventions](docs/conventions.md)
- [design considerations](docs/design.md)
