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

README
======

Overview
--------

The RTOS release contains a tool `prj` which is used for configuring the RTOS and (optionally) building systems based on the RTOS.
The `prj` tool can be found in the `x86_64-unknown-linux-gnu/bin` directory.

The RTOS itself is organised as a series of packages stored in the `share/packages` directory.
These are described in the following sections, and in further detail in their respective manual files.

For completeness, `LICENSE` contains the license under which this release is made available and `build_info` contains the specific build information used to uniquely identify the release.

Installation
-------------

To install the RTOS, simply extract the release archive in to a suitable location.
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

For example, to build the Kochab mutex demo system for the ARMv7-M STM32F4-Discovery board, run:

    prj -o kochab-mutex-out --search share/packages --no-project build machine-stm32f4-discovery.example.kochab-mutex-demo

Global Options
---------------

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

The `gen` sub-command takes as a single mandatory parameter the direct path to a system description file (PRX file).

The system description file specifies the system in a declarative manner.
In configuration mode, the description file only describes the configuration of the RTOS (and associated modules), rather than the full-system.

The format of the system description file is described in following sections.

An example usage of the `gen` sub-command is:

    $ prj --search share/packages --output rtos gen share/packages/machine-stm32f4-discovery/example/kochab-system.prx

This command will configure the RTOS based on the `kochab-system.prx` system description file, and generate the output files (including C and assembler source files, header files and linker scripts) into the `rtos` directory.

System Description File
------------------------

The system description file (or simply PRX file) is used to specify the modules of the system.
Note that when `prj` is used in configuration-only mode, this file specifies only RTOS-related modules.

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
At the most complex the module can be a set of multiple implementation files with a customised, script-based generation strategy.

When using the RTOS in a configuration only mode there are four modules that will be needed: `armv7m.ctxt-switch-preempt`, `armv7m.exception-preempt`, `armv7m.vectable`, and `armv7m.rtos-kochab`.

For more information on the `armv7m.ctxt-switch-preempt`, `armv7m.exception-preempt`, and `armv7m.vectable` modules, please see `share/packages/armv7m/armv7m-manual.md`.

The `armv7m.rtos-kochab` module is described in the following section.

`armv7m.rtos-kochab`
----------------------

The RTOS comes in a number of different *flavors*, each of varying complexity, code size and feature set.

The RTOS flavor *kochab* supports tasks, priority scheduling, mutexes with priority inheritance, semaphores, signals, and interrupt events which can cause task preemption and trigger the sending of signals.

For more information including configuration options, please see `share/packages/armv7m/rtos-kochab/documentation.pdf`.
