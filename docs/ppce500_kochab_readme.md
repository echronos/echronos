eChronos README
===============

Overview
--------

The eChronos release contains a tool `prj` which is used for configuring the RTOS and (optionally) building systems based on the RTOS.
The `prj` tool can be found in the `x86_64-unknown-linux-gnu/bin` directory.

eChronos itself is organised as a series of packages stored in the `share/packages` directory.
These are described in the following sections, and in further detail in their respective manual files.

For completeness, `LICENSE` contains the license under which this release is made available and `build_info` contains the specific build information used to uniquely identify the release.

Installation
-------------

To install eChronos simply extract the release archive in to a suitable location.
This can either be within a specific project directory, (e.g: `project/echronos`) or in a more general location (e.g: ~/local/echronos).

The `prj` binary is used to configure the RTOS, so should be installed somewhere such that it is convenient to use.
This can be done by adding `x86_64-unknown-linux-gnu/bin` to the PATH environment variable, creating a symbolic link to the `prj` binary, or simply using the full path within a build system.
E.g.: In `make`:

    PRJ := /path/to/echronos/x86_64-unknown-linux-gnu/bin/prj

Although the `prj` binary can be used anywhere on the file-system, the overall structure of the release should be maintained, as this is used to determine the location of `packages` directory.

`prj` should work on any modern Linux distribution.
It depends on zlib (version 1.2.0) and glibc (version 2.14).

Note: Mac OS X support is available, but not provided in this release.
Please advise if OS X support is desirable.

Usage
------

The RTOS can be used in two main ways.
In the full system mode the `prj` tool has full visibility to the source code base, and is used to build the full image.
In the configuration mode the `prj` tool is only used to configure the RTOS (and associated modules) rather than building the entire system firmware.

Future versions of the `prj` tools will take advantage of the full system mode to provide full system optimisations and checks, such as automatically sizing stacks.
Ideally projects that just target the RTOS will use the full system mode of `prj`.

Sub-commands
-------------

The `prj` tool is designed as a set of independent sub-commands.
The `gen` sub-command is used to generate source code for a system.
The `build` sub-command is used to build a system image, and the `load` sub-command is used to load a system image on to a target device (or simulator).

When using the `prj` tool for configuration mode only the `gen` sub-command is relevant.

Build guide
------------

This section describes the use of the `prj` tool's `build` sub-command.

Instructions below assume that the directory of `prj`, and also that of the required toolchain, are on the PATH.

The command to build a system takes the form:

    prj -o <out_dir> --search <packages_dir> --no-project build <package_name>

The package name takes the form of the dot-separated relative path of the system's .prx file from the packages directory.

For example, to build the kochab mutex demo system for QEMU PowerPC e500, run:

    prj -o kochab-mutex-out --search share/packages --no-project build machine-qemu-ppce500.example.kochab-mutex-demo

Global Options
---------------

The rest of this document assumes that `prj` will be used in configuration mode.

There are a number of global command line options available.
These options should be passed on the command line before the sub-command name.
E.g:

    prj --global-option sub-command

Not:

    prj sub-command --global-option

The `--project` option specifies a project description file.
The project description file is used to set project wide configuration options such as the output directory and search paths.
This option is primarily for use in full system mode, and is generally not required in configuration mode.
In full system mode the project description file defaults to `project.prj` (in the current working directory).
In configuration mode no project description file is used.

The `--no-project` option explicitly disables the use of a project file.
This is generally only useful in full system mode where a default project file is used.

The `--search-path` option allows additional package search paths to be specified on the command line.
This is generally not needed when using configuration mode.

The `--verbose` option enables additional output which may be useful when debugging.

The `--output` specifies the output directory.
This option is generally desirable when using `prj` from within another build system.

The default is `$project_output_dir/$system_name`, where `project_output_dir` is the project's output directory and `system_name` is the name of the system being built (or generated).
When no `project` is specified the `project_output_dir` will be `$pwd/out` (where `pwd` is the current working directory).

`gen` sub-command options
--------------------------

The `gen` sub-command takes a single mandatory parameter, which is the name of the system to build.
The system be either be a fully-qualified module name, or the direct path to a system description file (`.prx` file).

When using the tool in configuration mode the path to a system description file is generally used.

The system description file specifies the system in a declaration manner.
In configuration mode, the description file is only describing the configuration of the RTOS (and associated modules), rather than the full-system.

The format of the system description file is described in following sections.

An example usage of the `gen` sub-command is:

    $ prj gen --output rtos rtos.prx

This command will configure the RTOS based on the `rtos.prx` system description file, and generate the output files (including C and assembler source files, header files and linker scripts) into the `rtos` directory.

System Description File
------------------------

The system description file (or simply PRX file) is used to specify the system or in the case of the configuration only mode configure the RTOS related modules of the system.

Currently the system description is specified in XML, however additional formats are being considered and feedback is welcomed.

The document (root) element of the PRX file shall be `system`.
Currently only a single child element `modules` is specified.
Additional child elements may be available in the future.

The `modules` element may contain zero or more `module` child elements.
A `module` element represents the instantiation of a named module in the system.
Each `module` element may contain module specific sub-elements which configure that particular module for use within the system.
The `prj` considers the *system* to be composed of a set of configured modules.

`module` elements are processed by the `prj` tool in the order in which they appear in the PRX file.
When modules have configuration dependencies the dependent should be listed after the module on which it depends.

Each `module` element must have a `name` attribute, which names the module.
The RTOS comes with a number of built-in modules.
Custom modules may be created, however this is generally only needed when using `prj` in full system mode.

Modules may be of varying complexity.
At the simplest a module is simply a `.c` (or `.s`) file.
At the most complex the module can be a set of multiple implementation files with a customised, scripted-based generation strategy.

When using the RTOS in a configuration only mode there are three modules that will be needed: `ppce500.ctxt-switch`, `ppce500.vectable` and `ppce500.rtos-kochab`.
These are described in the following sections.

`ppce500.ctxt-switch`
----------------------

The `ppce500.ctxt-switch` module implements the low-level context switching operation.
This module does not have any configuration options, however as the RTOS depends on this module it must always be included in any system description.

`ppce500.vectable`
----------------------

The vectable module controls the generation of `vectable.s` and `default.ld`.

`vectable.s` implements the system vector table (i.e.: interrupt dispatch table) and implements the low-level `_entry` function, which is responsible for initialising the interrupt prefix and vector registers and jumping to `main`.

When using `prj` in configuration mode the user may prefer to use a custom hand-written `vectable.s` rather than the one generated by the `ppce500.vectable` module.

`default.ld` generates a linker-script that is suitable for use with the GNU linker.
Although it is possible to use an alternative linker script, care must be taken to ensure it contains the necessary symbol declarations, and to ensure that sections are grouped in an appropriate manner.

If using the generated `default.ld` is problematic, please discuss to determine suitable alternative approaches.

The `ppce500.vectable` module supports a number of (optional) configuration options.

The individual interrupt vectors supported by the PowerPC e500 may be configured through the following configuration options:

* critical_input
* machine_check
* data_storage
* instruction_storage
* external_input
* alignment
* program
* floating_point
* system_call
* aux_processor
* decrementer
* fixed_interval_timer
* watchdog_timer
* data_tlb_error
* instruction_tlb_error
* debug
* eis_spe_apu
* eis_fp_data
* eis_fp_round
* eis_perf_monitor

Each of these configuration has the type *C identifier*.
The C identifier type are all ASCII strings which are valid identifiers in the C language.
In all cases the identifier must refer to a function which will be called when the interrupt is taken.
The function can be a normal C function with zero arguments, returning `void`.
No checks are performed during configuration to ensure the specified interrupt handler function is available.
Subsequent linker errors will occur if the interrupt handler is incorrectly specified.

An empty string (the default) specfied for an interrupt handler will simply generate a vector that loops forever on itself.
Alternatively, setting the handler to string "undefined" will generate a vector that first creates a stack frame for the interrupted context and stores its registers there, before looping forever at location "undefined", which `vectable.s` places at address 0.

Finally, there is an option (`stack_size`) for specifying the initial stack size.
This stack is used by `main` up until the RTOS is actually started.
The default stack size is 4KiB (0x1000).

`ppce500.rtos-kochab`
----------------------

eChronos comes in a number of different *flavors*, each of varying complexity, code size and feature set.

The RTOS flavor *kochab* supports tasks, priority scheduling, mutexes with priority inheritance, semaphores, signals, and interrupt events which can cause task preemption and trigger the sending of signals.

There are a number of configuration options that should be set:

### `prefix`

The RTOS has an API that exports a number of functions (such as `signal_send_set`).
These APIs can be prefixed to provide a name-spacing.
The suggested prefix is `rtos_`, however anything can be chosen, which can help avoid namespace classes.

### `taskid_size`

The RTOS supports variables sized task identifiers.
This option sets the size of the identifier (in number of bits).
Only values of 8, 16 and 32 are supported.
Generally 8-bits (supporting up to 255 tasks) is sufficient, however there may be cases where using a larger size maps better with existing code or provides some performance benefits.

### `signalset_size`

The RTOS supports signal sets of varying sizes.
Possible values are 8, 16 and 32.
The signal set size places a limit on the number of individual signals that are available.

### `tasks`

The configuration should also include a `tasks` element, with a number of `task` child elements.
The `task` elements define each of the tasks in the system.
Each task has the following configuration elements:

#### `name`

The name of the task.
This should be a valid C identifier.
Task names must be unique within the system.

#### `function`

The entry point for the task.
The entry point should be a C function that takes zero arguments and never returns.

#### `priority`

The priority for the task.
A higher number denotes a higher priority.

#### `stack_size`

Size of the stack allocated to this task.

### `mutexes`

The `mutexes` element should contain `mutex` child elements, which define each of the mutexes in the system.

#### `name`

Each `mutex` element contains just a single child element, `name`, which should be a valid C identifier specifying the name of the mutex.

### `semaphores`

The `semaphores` element should contain `semaphore` child elements, which define each of the semaphores in the system.

#### `name`

Each `semaphore` element contains just a single child element, `name`, which should be a valid C identifier specifying the name of the semaphore.

### `signal_labels`

The `signal_labels` element should contain `signal_label` child elements, which define each of the signals in the system.

#### `name`

Each `signal` element contains just a single child element, `name`, which should be a valid C identifier specifying the name of the signal.
