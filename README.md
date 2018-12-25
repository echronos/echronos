<!--
     eChronos Real-Time Operating System
     Copyright (c) 2017, Commonwealth Scientific and Industrial Research
     Organisation (CSIRO) ABN 41 687 119 230.

     All rights reserved. CSIRO is willing to grant you a licence to the eChronos
     real-time operating system under the terms of the CSIRO_BSD_MIT license. See
     the file "LICENSE_CSIRO_BSD_MIT.txt" for details.

     @TAG(CSIRO_BSD_MIT)
-->

Core repository:
[![Core Build Status on Linux](https://travis-ci.org/echronos/echronos.svg?branch=master)](https://travis-ci.org/echronos/echronos)
[![Core Build Status on Windows](https://ci.appveyor.com/api/projects/status/u0l9tcx3r8x9fwj0/branch/master?svg=true)](https://ci.appveyor.com/project/stefangotz/echronos/branch/master)

Client repository:
[![Client Build Status on Linux](https://travis-ci.org/echronos/test-client-repo.svg?branch=master)](https://travis-ci.org/echronos/test-client-repo)
[![Client Build Status on Windows](https://ci.appveyor.com/api/projects/status/wbyntsf0a5crcl62/branch/master?svg=true)](https://ci.appveyor.com/project/stefangotz/test-client-repo/branch/master)

---

# Table of Contents

- [Overview](#overview)
- [Quick-start for Linux and Windows](#quick-start-for-linux-and-windows)
- [Quick-start for ARMv7m](#quick-start-for-armv7m)
- [Quick-start for PowerPC](#quick-start-for-powerpc-e500-targets)
- [Documentation](#documentation)
- [Software model](#software-model)
- [Common development tasks](#common-development-tasks)
- [Repository structure](#repository-structure)
- [Project conventions](docs/conventions.md)
- [Design considerations](docs/design.md)

# Yes, We Are Open for Business

If you have any questions, send us an e-mail to echronos@trustworthy.systems or tweet at us [@echronosrtos](https://twitter.com/echronosrtos).

If there is something in the project that you think is worth improving, please create a [github issue](https://github.com/echronos/echronos/issues).

Of course, we are also keen on your changes and contributions if you have any - [here is a primer](CONTRIBUTING.md).

# Overview

The eChronos RTOS is a real-time operating system (RTOS) originally developed by NICTA/Data61 and Breakaway Consulting Pty. Ltd.

It is intended for tightly resource-constrained devices without memory management units and virtual memory support.
To this end, the RTOS code base is designed to be highly modular and configurable on multiple levels, so that only the minimal amount of code necessary is ever compiled into a given system image.

Available implementations currently target ARM Cortex-M4 and PowerPC e500.
The RTOS also runs on POSIX platforms (e.g., Linux, MacOS-X, Windows with cygwin or MinGW) for rapid prototyping.


# Quick-start for Linux and Windows

Get a first impression of how the RTOS is being used without delving into the details of any embedded hardware platform.
This section covers how to build and run a simple application on top of the RTOS as a regular Linux or Windows process.

## Download (Linux and Windows)

Download the [latest _posix_ release](https://github.com/echronos/echronos/releases) and unpack it in a directory of your choice.

The project makes frequent releases, based on improvements flowing into the master branch of the code repository.
You'll notice that releases are not just snapshots of the code repository.
Rather, they provide just the bits and pieces needed for individual target platforms, with some of the code and tools already pre-built.

The _posix_ release is aimed at running the RTOS on top of any POSIX host operating system, such as Linux or Windows (with Cygwin or MinGW).

## Prerequisites (Linux and Windows)

You need the following tools installed and ready to go:

- Python 3
- GCC compiler + GNU binutils

On Linux, use your package manager to install these tools.
On Windows, obtain and install Python 3 from [python.org](https://www.python.org) and install either [Cygwin](https://cygwin.com) or [MSys2](http://www.msys2.org/) including the GCC compiler.

## Build (Linux and Windows)

The following commands build a simple version of the eChronos RTOS together with a small example application:

    cd eChronos-posix-VERSION
    ./bin/prj.sh build posix.acamar

On Windows, run `prj\prj` instead of `./prj/prj.sh`.
This produces the binary `out/posix/acamar/system` (`system.exe` on Windows).

## Run (Linux and Windows)

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
The [Variants and Components](#variants-and-components) section has more information on this topic.

## Where to from here?

The rest of this README covers ARM and PowerPC quick-starts as well as all basic RTOS concepts and how to make use of them.
It makes use of the full [source code repository](https://github.com/echronos/echronos/), not just a release as the Quick-start Guide did above.


# Quick-start for ARMv7m

## Prerequisites (ARMv7m)

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

To obtain `qemu-system-arm` for emulating ARM systems, read the README.md for [our QEMU fork](https://github.com/echronos/qemu/) (make sure to use the `master` branch).

On most linux distributions, it will be simplest to use the binary releases included alongside the QEMU fork repository - see 'Using the binaries' section of the QEMU README.md.

## Running an example system (ARMv7m)

Build and run an example system for the RTOS variant *Gatria* on QEMU-emulated ARMv7-M (LM3S):

    cd echronos
    prj/app/prj.py build machine-qemu-simple.example.gatria-system

    # Run the generated system in qemu (press `ctrl-c` to close QEMU after it is finished)
    qemu-system-arm -M lm3s6965evb -nographic -semihosting -S -s --kernel out/machine-qemu-simple/example/gatria-system/system

    # To connect and view debug output run gdb in another shell prompt
    arm-none-eabi-gdb -ex "target remote :1234" out/machine-qemu-simple/example/gatria-system/system
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


# Quick-start for PowerPC e500 Targets

## Prerequisites (PowerPC e500)

Obtain the source code with the command

    git clone --depth=1 https://github.com/echronos/echronos.git

To obtain the `powerpc-linux-gnu` GNU toolchain for building the RTOS for PowerPC e500 on Ubuntu Linux systems, run:

    sudo apt-get install gcc-powerpc-linux-gnu

To obtain the `qemu-system-ppc` emulator for running the [`machine-qemu-ppce500`](packages/machine-qemu-ppce500) systems on Ubuntu Linux systems, run:

    sudo apt-get install qemu-system-ppc

To obtain, build, and install `powerpc-linux-gdb` for debugging PowerPC e500 systems, run:

    wget https://ftp.gnu.org/gnu/gdb/gdb-7.12.tar.xz
    tar xaf gdb-7.12.tar.xz
    cd gdb-7.12
    ./configure --target=powerpc-linux --prefix=/usr/
    make -s
    sudo make -s install

## Running an example system (PowerPC e500)

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


# Documentation

Basic RTOS concepts and usage are documented in README file and the [`docs`](docs) directory.

More detailed documentation for the `x.py` tool can be found inside [`x.py`](x.py) itself.
More detailed documentation for the `prj` tool can be found in [`prj/manual/prj-user-manual.md`](prj/manual/prj-user-manual.md).

Pregenerated RTOS API manuals can be found on the [eChronos GitHub wiki](https://github.com/echronos/echronos/wiki) or you can [build them yourself](#building-user-documentation).


# Software Model

The software model and structure of the RTOS is governed by two stages of customization.

In the first stage, features, in the form of *components*, are customized for and composed into *variants* of the RTOS such that each variant has a specific feature set.
This stage is supported by the `x.py` tool.

In the second stage, the RTOS variant is customized to the specific properties and requirements of a specific application.
Although this customization is limited to the functionality provided by the given variant, it controls details such as the number of tasks required by the application.
This stage is supported by the `prj` tool.

The two stages can optionally be separated by deploying a *product release* to an application project.
The application project is then only exposed to the second stage and the variant and functionality of the RTOS they require.

The following sections cover these concepts in more detail.

## Variants and Components

The RTOS comes in a number of different *variants*, each offering a specific set of features for a particular platform.

For example:

* The RTOS variant *Rigel* supports tasks, co-operative round-robin scheduling, mutexes, signals, and interrupt events which can trigger the sending of signals.
It is available for QEMU-emulated ARMv7-M.

* The RTOS variant *Kochab* supports tasks, preemptive priority scheduling, mutexes with priority inheritance, semaphores, signals, and interrupt events which can cause task preemption and trigger the sending of signals.
It is available for the ARMv7e-M STM32F4-Discovery board, QEMU-emulated ARMv7-M, and QEMU-emulated PowerPC e500.

Features are implemented as individual *components* in individual C files.
Unlike typical C-based software, the RTOS does not compile components individually and later link them into a single binary.
Instead, the RTOS `x.py` tool merges the component files of a variant into a single C file called the *RTOS module*.
The feature set of each RTOS variant is specified within the `x.py` tool itself.
This allows for better compile-time optimizations.

The `x.py` tool also supports building *product releases* of the RTOS.
A product release bundles a set of variants and target platforms tailored for a certain application product that builds on the RTOS.
Thus, the application product sees only what it needs without being needlessly exposed to all features and platforms supported by the RTOS.
The variants and platforms contained in a release are defined by [`release_cfg.py`](release_cfg.py).


## Systems, Modules and Packages

An RTOS *system* encompasses the entirety of the OS and an application on top of it.
It consists in particular of:
- the OS in the form of a variant of the RTOS with a feature set suitable for the application (e.g., which form of scheduling is supported)
- a *system configuration* that tailors the variant to the specific application instance (e.g., how many task or mutexes the application requires)
- the application code itself

Systems are built via the `prj` tool which implements the RTOS build system.
At the build system level, systems are composed of *modules* (such as the RTOS module), so modules provide the unit of composition and reusability.

In its simplest form, a module is a C file and that is usually all that applications need to know about modules.
However, modules can consist of the following elements:

* Entity definition file named `entity.py` or `<module_name>.py`, specifying the module contents and customization options by providing a `module` Python object.
* C and header file named `<module_name>.c/h`, providing the public interface and its implementation of the module in C
* Assembly file named `<module_name>.s`, providing the module functionality as assembly code
* Linker script named `<module_name>.ld`, specifying linker commands for linking the system (not just the module)
* XML Configuration schema (as a standalone file or integrated into the entity definition script or source code files), specifying the configuration parameters supported by the module
* Builder module script `<module_name>.py`, defining a `system_build()` function to be executed in order for the system to be built.
The presence of this function distinguishes the module as a Builder module.

A system is statically defined and configured in its *system configuration* in a `.prx` file.
This is an XML file that lists the modules that make up a system and provides configuration parameters for each of the modules.
The `.prx` file includes a static declaration of all the RTOS resources used by the system, including all tasks, mutexes, semaphores, interrupt handlers, etc.
The `prj` tool reads `.prx` files and composes, compiles, and links all the code to produce a system binary.

A complete system typically consists of

* an RTOS module
* modules dictating the build process for the target platform
* one or more modules containing platform-specific assembly code needed by the RTOS variant to implement low-level OS functionality
* one or more modules containing user-provided code that implement the application functionality

For example, the Kochab RTOS example system for QEMU-emulated PowerPC e500 (defined in [`packages/machine-qemu-ppce500/example/kochab-system.prx`](packages/machine-qemu-ppce500/example/kochab-system.prx)) contains the following modules:

* `ppce500.rtos-kochab`, the Kochab variant of the RTOS for ppce500.
* `ppce500.build` and `ppce500.default-linker`, which define building and linking for ppce500.
* `ppce500.interrupts-util` and `ppce500.vectable`, which provide assembly-level RTOS code for ppce500.
* `ppce500.debug` and `generic.debug`, which define stubs for debug printing.
* `machine-qemu-ppce500.example.kochab-test`, the user-provided test program for Kochab QEMU-emulated PowerPC e500.

On the file system, modules are grouped into *packages*, allowing modules to be organised based on common characteristics (such as platform or intended usage).
For example, the PowerPC e500 RTOS variant modules are grouped together with the `build`, `default-linker`, `interrupts-util` and `vectable` modules in the [`ppce500`](packages/ppce500) package.
As another example, platform-agnostic RTOS example code such as the `kochab-mutex-test` and `timer-test` modules are grouped together in the [`rtos-example`](packages/rtos-example) package.

## Tool support

As described above, `x.py` provides the means to generate all the different RTOS variants and `prj` provides the means to combine an RTOS module with other modules to produce a system binary.

There are two main steps in building an RTOS-based system from the RTOS repository.

Step 1: Generate the RTOS variants.

     ./x.py build packages

This generates all the RTOS variants.
For each variant specified in `x.py` it finds the appropriate components and combines them into the RTOS variant.
The resulting variant can be found in `packages/<platform>/rtos-<variant>`.

Please look at the documentation inside `x.py` for more information on the tool.


Step 2: Build a system.

     prj/app/prj.py build <system name>

This finds the appropriate `.prx` file, combines the required modules and generates a system binary.
`prj` can be further configured using the top level [`project.prj`](project.prj) file, which specifies tasks that are automatically performed when `prj` is run as well as the locations to look for modules and `.prx` configuration files.

As a convenience `prj` can be configured to automatically regenerate RTOS modules whenever it is run.
This is done by including the following line in the `project.prj` file:

     <startup-script>./x.py build partials --allow-unknown-filetypes</startup-script>

Please see `prj/manuals/prj-user-manual` for more information on the `prj` tool.


# Common Development Tasks

## Developing a RTOS variant

To generate the code for all available RTOS variants:

    ./x.py build packages

The resulting RTOS code is placed into `packages/<platform>/rtos-<variant>/`.

The RTOS variants themselves are specified as lists of components within `x.py` itself.
Adding a new RTOS variant means adding the appropriate list entries to `x.py`, and adding new component code in the `components` directory structure if necessary.

Please see the existing component code under [`components`](components) for examples on how to develop RTOS components.


## Building user documentation

Manuals for the main RTOS variants are built with the command `./x.py build docs`.
On Linux, this depends on the tools _wkhtmltopdf_ and _pandoc_ to be installed.

The manuals are created in `packages/<platform>/rtos-<variant>/documentation.*` in HTML, Markdown, and PDF.

Like the RTOS component code, the user manual content is componentized so that only the user documentation for components present in a particular RTOS variant appears in the user manual for that variant.

See `components/*/docs.md` for examples of componentized user documentation.


## Building the `prj` tool

To build just the `prj` tool binary for stand-alone use:

    # The `prj` binary is output to `prj_build_x86_64-unknown-linux-gnu/prj`
    ./x.py build prj


## Configuring and building a system

To build a system, use the `prj build` sub-tool of `prj`, supplying the system name as argument.
The name of the system is the basename of its `.prx` file, minus its `.prx` extension, appended to a dot-separated string indicating the sub-package location in which it can be found.

For example, to build the system whose `.prx` file is located at [`packages/machine-qemu-ppce500/example/kochab-timer-demo.prx`](packages/machine-qemu-ppce500/example/kochab-timer-demo.prx), from the top level of the repository:

    prj/app/prj.py build machine-qemu-ppce500.example.kochab-timer-demo

Alternatively, assuming the `prj` tool binary is on the PATH:

    prj build machine-qemu-ppce500.example.kochab-timer-demo


## Building a product release

To build all product releases, run:

    ./x.py build prj
    ./x.py build docs
    ./x.py build partials
    ./x.py build release

The releases are defined in the top-level [`release_cfg.py`](release_cfg.py) script and are generated by `x.py` into `release/*.tar.gz`.

Each product release contains:

* a `LICENSE` file
* a `README.md` file containing a brief introduction to the release
* a `build_info` file containing the commit hash of the RTOS repository from which the release was built
* a `share` directory, containing the package directory structure containing all the released packages
* a pre-built `prj` binary for the host platform targeted by the release

Note that any user manuals available for each RTOS variant in the release can be found at `share/packages/<platform>/rtos-<variant>/documentation.*` in HTML, Markdown and PDF.


## Using a product release

After unpacking the product release, create a `project.prj` file for your software project in order to be able to use the `prj` tool that comes packaged with the release.

A minimal `project.prj` contains, at the very least, a `search-path` entry to give `prj` a hint as to where to find modules.
Here is a example that points to its relative location from the root of the release:

    <?xml version="1.0" encoding="UTF-8" ?>
    <project>
        <search-path>share/packages</search-path>
    </project>

After this, the `prj` binary packaged with the release can be used in the same way as in the RTOS repository.
Additional application code can be placed in other directories as long as they are included as search paths in the project.prj file.

For example, to build the Kochab timer demo system provided with the PowerPC e500 release of the RTOS for Linux, run:

    x86_64-unknown-linux-gnu/bin/prj build machine-qemu-ppce500.example.kochab-timer-demo

Note that the PATH environment variable needs to be set up manually so that it includes the tools invoked by `prj`.


# Repository Structure

The [`components`](components) directory contains the RTOS component source and documentation.

The [`packages`](packages) directory provides various platform-specific RTOS modules and other source modules.
RTOS modules generated by `x.py` are output to locations within the `packages` directory structure.

The [`x.py`](x.py) tool provides an interface for:
* RTOS module and product release generation (`x.py build`),
* testing (`x.py test`).

Much of `x.py`'s underlying implementation resides in the [`pylib`](pylib) directory, and its self-test suite (`x.py test x`) is implemented in [`x_test.py`](x_test.py).

The [`prj`](prj) directory contains everything related to the `prj` tool, which provides an interface for configuring and building systems as compositions of source modules.

The `release` directory is created by `x.py`, and contains all releasable artifacts such as product releases.

The [`external_tools`](external_tools) and [`tools`](tools) directories contain external tools committed to the repository in order for the repository to be self-contained in its ability to provide everything that is needed for development.
See [`external_tools/LICENSE.md`](external_tools/LICENSE.md) and [`tools/LICENSE.md`](tools/LICENSE.md) for license information regarding the contents of these directories.

The [`pm`](pm) directory contains project management related meta-data.
This meta-data is crucial for documenting that our internal development processes are correctly followed, as well as providing a record for external audit.
The feature documentation for development tasks resides in [`pm/tasks`](pm/tasks), and their reviews can be found under [`pm/reviews`](pm/reviews).

The [`docs`](docs) directory contains various release documentation-related content, including templates for auto-generated manuals and top-level README files for product releases.

The [`unit_tests`](unit_tests) directory contains unit-test code, including a model of the RTOS schedulers implemented in Python.
