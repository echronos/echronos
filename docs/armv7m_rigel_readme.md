eChronos README
===============

Release overview
-----------------

The RTOS release contains a tool `prj` which is used for configuring
the RTOS and (optionally) building systems based on the RTOS. The
`prj` tool can be found in the `x86_64-unknown-linux-gnu/bin`
directory.

The RTOS itself is organised as a series of packages stored in the
`share/packages` directory. These are described in the following
sections.

For completeness, `LICENSE` contains the license under which this
release is made available and `build_info` contains the specific build
information used to uniquely identify the release.

Installation
-------------

To install the RTOS simply extract the release archive in to a
suitable location. This can either be within a specific project
directory, (e.g: `project/rtos`) or in a more general location (e.g:
~/local/rtos).

The `prj` binary is used to configure the RTOS, so should be installed
somewhere such that it is convenient to use. This can be done by adding
`x86_64-unknown-linux-gnu/bin` to the PATH environment variable, creating
a symbolic link to the `prj` binary, or simply using the full path within
a build system. E.g.: In `make`:

    PRJ := /path/to/rtos/x86_64-unknown-linux-gnu/bin/prj

Although the `prj` binary can be used anywhere on the file-system, the
overall structure of the release should be maintained, as this is used
to determine the location of `packages` directory.

`prj` should work on any modern Linux distribution. It depends on zlib
(version 1.2.0) and glibc (version 2.14).

Note: Mac OS X support is available, but not provided in this release.
Please advise if OS X support is desirable.

Usage
------

The RTOS can be used in two main ways. In the full system mode the
`prj` tool has full visibility to the source code base, and is used to
build the full image. In the configuration mode the `prj` tool is only
used to configure the RTOS (and associated modules) rather than
building the entire system firmware.

Future versions of the `prj` tools will take advantage of the full
system mode to provide full system optimisations and checks, such as
automatically sizing stacks. Ideally projects that just target the
RTOS will use the full system mode of `prj`.

The rest of this document assumes that `prj` will be used in
configuration mode.

Sub-commands
-------------

The `prj` tool is designed as a set of independent sub-commands.  The
`gen` sub-command is used to generate source code for a system. The
`build` sub-command is used to build a system image, and the `load`
sub-command is used to load a system image on to a target device (or
simulator).

When using the `prj` tool for configuration mode only the `gen`
sub-command is relevant.

Global Options
---------------

There are a number of global command line options available. These
options should be passed on the command line before the sub-command
name. E.g:

    prj --global-option sub-command

Not:

    prj sub-command --global-option

The `--project` option specifies a project description file. The
project description file is used to set project wide configuration
options such as the output directory and search paths. This option is
primarily for use in full system mode, and is generally not required
in configuration mode. In full system mode the project description
file defaults to `project.prj` (in the current working directory). In
configuration mode no project description file is used.

The `--no-project` option explicitly disables the use of a project
file. This is generally only useful in full system mode where a
default project file is used.

The `--search-path` option allows additional package search paths to
be specified on the command line. This is generally not needed when
using configuration mode.

The `--verbose` option enables additional output which may be useful
when debugging.

The `--output` specifies the output directory. This option is
generally desirable when using `prj` from within another build system.

The default is `$project_output_dir/$system_name`, where
`project_output_dir` is the project's output directory and
`system_name` is the name of the system being built (or
generated). When no `project` is specified the `project_output_dir`
will be `$pwd/out` (where `pwd` is the current working directory).

`gen` sub-command options
--------------------------

The `gen` sub-command takes a single mandatory parameter, which is the
name of the system to build. The system be either be a fully-qualified
module name, or the direct path to a system description file (`.prx`
file).

When using the tool in configuration mode the path to a system
description file is generally used.

The system description file specifies the system in a declaration
manner.  In configuration mode, the description file is only
describing the configuration of the RTOS (and associated modules),
rather than the full-system.

The format of the system description file is described in following
sections.

An example usage of the `gen` sub-command is:

    $ prj gen --output rtos rtos.prx

This command will configure the RTOS based on the `rtos.prx` system
description file, and generate the output files (including C and
assembler source files, header files and linker scripts) into the
`rtos` directory.

System Description File
------------------------

The system description file (or simply PRX file) is used to specify
the system or in the case of the configuration only mode configure the
RTOS related modules of the system.

Currently the system description is specified in XML, however
additional formats are being considered and feedback is welcomed.

An example system description file is available at:
`share/packages/machine-qemu-simple/example/rigel-config-only.prx`

The document (root) element of the PRX file shall be `system`.
Currently only a single child element `modules` is specified.
Additional child elements may be available in the future.

The `modules` element may contain zero or more `module` child
elements. A `module` element represents the instantiation of a named
module in the system. Each `module` element may contain module
specific sub-elements which configure that particular module for use
within the system. The `prj` considers the *system* to be composed of
a set of configured modules.

`module` elements are processed by the `prj` tool in the order in
which they appear in the PRX file. When modules have configuration
dependencies the dependent should be listed after the module on which
it depends.

Each `module` element must have a `name` attribute, which names
the module. The RTOS comes with a number of built-in modules.
Custom modules may be created, however this is generally only
needed when using `prj` in full system mode.

Modules may be of varying complexity. At the simplest a module
is simply a `.c` (or `.s`) file. At the most complex the module
can be a set of multiple implementation files with a customised,
scripted-based generation strategy.

When using the RTOS in a configuration only mode there are
three modules that will be needed: `armv7m.ctxt-switch`,
`armv7lm.vectable` and `armv7m.rtos-rigel`. These are described
in the following sections.

`armv7m.ctxt-switch`
----------------------

The `armv7m.ctxt-switch` module implements the low-level context
switching operation. This module does not have an configuration
options, however as the RTOS depends on this module it must always be
included in any system description.

`armv7m.vectable`
----------------------

The vectable module controls the generation of `vectable.s` and
`default.ld`, as well as providing the `bitband.h` header file,
which is used by the RTOS.

`vectable.s` implements the system vector table (i.e.: interrupt
dispatch table and implements the low-level `_entry` function,
which is responsible for initialising `.data` and `.bss` before
jumping to `main`.

When using `prj` in configuration mode the user may prefer to
use a custom hand-written `vectable.s` rather than the one generated
by the `armv7m.vectable` module.

`default.ld` generates a linker-script that is suitable for use with
the GNU linker. Although it is possible to use an alternative
linker script, care must be taken to ensure it contains the necessary
symbol declarations, and to ensure that sections are grouped in
an appropriate manner.

If using the generated `default.ld` is problematic, please
discuss to determine suitable alternative approaches.

The `bitband.h` header defines a number of special macros which are
used to simplify the use of the ARM *bitband* feature. The *bitband*
feature allows atomic access to individual bits within words, and
forms a critical piece of the RTOS implementation. In addition to
the macro expansion used by the compiler, uses of the macro are
also parsed by the `armv7m.vectable` module and used to populate
the `default.ld` file with the appropriate symbol aliases.

Note: When using `prj` in configuration only mode the `bitband.h`
header can not be used by code other than the RTOS itself.

The `armv7m.vectable` module supports a number of (optional)
configuration options.

The individual exceptions may be configured through the following
configuration options:

* exception_reset
* nmi
* hardfault
* busfault
* usagefault
* svcall
* debug_monitor
* pendsv
* systick

Each of these configuration has the type *C identifier*, and defaults
to `reset`. The C identifier type are all ASCII strings which are valid
identifiers in the C language. In all cases the identifier must refer
to a function which will be called when the exception is invoked. The
function can be a normal C function with zero arguments, returning `void`.
No checks are performed during configuration to ensure the specified
exception handler function is available. Subsequent linker errors will
occur if the exception handler is incorrectly specified.

Note: The `vectable.s` file implements the default `reset` exception handler.

The other options available for the `armv7m.vectable` module control
the memory layout. The correct values for these items generally
require consultation with the chip reference manual. The following
values are configurable (defaults appear in parens):

* flash_load_addr (0x0)
* code_addr (0x0)
* data_addr (0x0)
* bitband_base (0x2000.0000)
* bitband_size (0x100.0000)
* bitband_alias (0x2200.0000)

Finally, there is an option (`stack_size`) for specifying the initial
stack size.  This stack is used by `main` up until the RTOS is
actually started. The default stack size is 4KiB (0x1000).

`armv7m.rtos-rigel`
----------------------

The RTOS comes in a number of different *flavors*, each of varying complexity,
code size and feature set.
This section documents the *rigel* RTOS flavor.

This RTOS supports tasks, round-robin scheduling, signals and interrupt
events which can trigger the sending of signals.

There are a number of configuration options that should be set
for the RTOS.

### `prefix`

The RTOS has an API that exports a number of functions (such as
`signal_send_set`). These APIs can be prefixed to provide a
name-spacing. The suggested prefix is `rtos_`, however anything can
be chosen, which can help avoid namespace classes.

### `taskid_size`

The RTOS supports variables sized task identifiers. This option sets
the size of the identifier (in number of bits). Only values of 8, 16
and 32 are supported. Generally 8-bits (supporting up to 255 tasks) is
sufficient, however there may be cases where using a larger size maps
better with existing code or provides some performance benefits.

### `signalset_size`

The RTOS supports signal sets of varying sizes. Possible values
are 8, 16 and 32. The signal set size places a limit on the number
of individual signals that are available.

### `interrupteventid_size`

Interrupt event identifiers may also vary in size. Valid values are 8, 16
and 32.

### `num_tasks`

The RTOS is designed for static systems, so the number of tasks
must be declared in advance.

Note: The tasks themselves must also be explicitly declared, so
to some extent this option is redundant. This should be improved
in an upcoming release.

### `num_interrupt_events`

This represents the maximum number of interrupt events in the system.

### `tasks`

The configuration should also include a `tasks` element, with a number
of `task` child elements. The `task` elements define each of the tasks
in the system. Each task has the following configuration elements:

#### `idx`

The task's identifier. Note: This may be automatically assigned in
future version. Task identifiers must be unique within the system.

#### `name`

The name of the task. This should be a valid C identifier. Task names
must be unique within the system.

#### `entry`

The entry point for the task. The entry point should be a C function
that takes zero arguments and never returns.

#### `stack_size`

Size of the stack allocated to this task.

### `interrupt_events`

The configuration should include an `interrupt_events` element, with
a number of `interrupt_event` child elements. The `interrupt_event` element
defines each of the individual interrupt events. Each interrupt event has
the following configuration elements:

#### `idx`

The interrupt event's identifier. Note: This may be automatically assigned in
future version. interrupt event identifiers must be unique within the system.

#### `name`

The name of the interrupt event. This should be a valid C identifier. interrupt
event names must be unique within the system.

#### `task`

When an interrupt event is raised it sends a signal set to a specific task.
The `task` configuration element specifies the task identifier.

#### `sig_set`

The signal set to send to the specified task when the interrupt event is raised.
