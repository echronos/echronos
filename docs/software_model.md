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
It is available for the ARMv7-M STM32F4-Discovery board, and QEMU-emulated PowerPC e500.

Features are implemented as individual *components* in individual C files.
Unlike typical C-based software, the RTOS does not compile components individually and later link them into a single binary.
Instead, the RTOS `x.py` tool merges the component files of a variant into a single C file called the *RTOS module*.
The feature set of each RTOS variant is specified within the `x.py` tool itself.
This allows for better compile-time optimizations.

The `x.py` tool also supports building *product releases* of the RTOS.
A product release bundles a set of variants and target platforms tailored for a certain application product that builds on the RTOS.
Thus, the application product sees only what it needs without being needlessly exposed to all features and platforms supported by the RTOS.
The variants and platforms contained in a release are defined by [`release_cfg.py`](../release_cfg.py).


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

For example, the Kochab RTOS example system for QEMU-emulated PowerPC e500 (defined in [`packages/machine-qemu-ppce500/example/kochab-system.prx`](../packages/machine-qemu-ppce500/example/kochab-system.prx)) contains the following modules:

* `ppce500.rtos-kochab`, the Kochab variant of the RTOS for ppce500.
* `ppce500.build` and `ppce500.default-linker`, which define building and linking for ppce500.
* `ppce500.interrupts-util` and `ppce500.vectable`, which provide assembly-level RTOS code for ppce500.
* `ppce500.debug` and `generic.debug`, which define stubs for debug printing.
* `machine-qemu-ppce500.example.kochab-test`, the user-provided test program for Kochab QEMU-emulated PowerPC e500.

On the file system, modules are grouped into *packages*, allowing modules to be organised based on common characteristics (such as platform or intended usage).
For example, the PowerPC e500 RTOS variant modules are grouped together with the `build`, `default-linker`, `interrupts-util` and `vectable` modules in the [`ppce500`](../packages/ppce500) package.
As another example, platform-agnostic RTOS example code such as the `kochab-mutex-test` and `timer-test` modules are grouped together in the [`rtos-example`](../packages/rtos-example) package.

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
`prj` can be further configured using the top level [`project.prj`](../project.prj) file, which specifies tasks that are automatically performed when `prj` is run as well as the locations to look for modules and `.prx` configuration files.

As a convenience `prj` can be configured to automatically regenerate RTOS modules whenever it is run.
This is done by including the following line in the `project.prj` file:

     <startup-script>./x.py build partials --allow-unknown-filetypes</startup-script>

Please see `prj/manuals/prj-user-manual` for more information on the `prj` tool.
