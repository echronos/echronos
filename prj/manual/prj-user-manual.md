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

%title Project Manager
%docid 5xDTZm
%version 1-draft1

Introduction
--------------

Successfully completing a firmware project requires more than just a suitable RTOS kernel.
Bundled with the RTOS kernel is a tool that is recommended for managing projects that incorporate the kernel.
This document describes how to use the Project Manager tool, called **prj**, to help manage your firmware project.


Concepts
----------


### Project

The top-level entity handled by **prj** is a *project*.
A firmware project includes all the source needed to produce some set of firmware deliverables.
Usually the firmware deliverables will be one or more firmware images that are to be installed on a device.
Additionally there are likely to be additional artifacts such as test reports and documentation that must also be produced by a firmware project.

The **prj** tool is designed to assist users in building the firmware images that must be released, and any other firmware images that are used for testing the firmware.

A *project* is defined in a *project definition file*, which is an XML formatted file that describes all the aspects of the project.
When invoked **prj** searches for a project definition file.
By default, it looks for a file name `project.prj` in the current working directory, but a project file may be specified explicitly on the command line.

An example project file:

    <project>
      <startup-script>do_something.sh</startup-script>
      <search-path>stdlib</search-path>
    </project>

The document must have a top-level `project` element.
The `project` element can have any number of `search-path` element children.
Each `search-path` element defines a path in which to search for entities.
Order is important; paths will be searched in the same order as in the definition file.
For any operation using the search-path, searching will stop on the first match.
If an entity exists in multiple search-paths the first matching entity is returned from any search.

The `project` element can have a number of `startup-script` elements, each of which defines a startup script.
All startup scripts are run immediately after the project file is parsed and before any other build actions are run.
Multiple `start-script` elements may be defined.
Each will be run in the order defined.
If a startup script fails (i.e.: has a non-zero exit code) `prj` will exit with an error.
The start script mechanism is designed to provide flexibility to the system designer and avoid the need for unnecessary wrapper scripts.
Some example usages of startup scripts include code generation, checking things out from revision control, or project wide sanity checks.


### Systems

*System* is the name given to some code and data the runs on some target hardware.
A project may (and likely will) have multiple associated systems.
For example there is the production system that will end up running on the device, and there may also be a debug or test system that is also used during development.
Additionally, there may be systems that are used specifically for unit-testing individual code modules.

A *system* is usually defined in a *system definition file*, which is an XML formatted file that describes the system.

As there are often very similar systems within a project, **prj** also supports *base systems* and *variants*.
A *base system* is a parameterised system.
For example, a base system may support different debug levels.
In such a case there would be a `debug_level` parameter, which would have a set of possible values (such as `production` and `debug`).
The values that a parameter can take must be specified in the base system, and can only be enumerations, rather than arbitrary integers or strings.
FIXME: This is currently unimplemented; more details to be defined.

A *variant system* is a particular instantiation of a base system, with values chosen for all parameters.
Variant systems don't have a system definition file, but rely on the underlying base system's definition file.
A variant system is chosen on the command line (or through the GUI).
FIXME: Currently the mechanism for defining and using variant systems is undefined.

The number of variant systems that a base system supports is the Cartesian product of all the parameter sets.

**Future:** in the future it may be possible to specify additional constraints to restrict the available combinations of parameters.

A project is able to support multiple systems and base-systems.

An example system definition file:

    <system>
     <modules>
      <module name="posix-build" />
      <module name="rtos-acamar">
        <taskid_size>8</taskid_size>
           <num_tasks>2</num_tasks>
      </module>
      <module name="example/acamar-test" />
     </modules>
    </system>

The system definition file has a top-level `system` element.
The `system` element should contain a `module` element.
The `modules` element should contain 1 or more `module` elements.
Each `module` element must have a `name` attribute.
The `prj` tool will search for the module entity based on the `name` attribute.
`module` entities are described in the following section.
The `module` element can have child elements that define the way in which the module is configured.

### Module

A *system* is composed of multiple individual modules.
A *module* is some set of functionality implemented in code.
Generally a module will consist of a C implementation file and a header file that defines the interface.
A module can consist of multiple implementation or header files.
Additionally a module may have zero implementation files or zero header files.
Modules provide the unit of composition and reusability.
Modules must be reused as an indivisible unit, rather than reusing files within the module.

A module may export configuration parameters.
Configuration parameters enhance the reusability of a module.
The system definition file provides the value for a configuration parameter.

Modules are defined in a *module definition file*.
The *module definition file* may be an XML file or a Python script.
For simple modules a *module definition file* can be omitted and the module inferred implicitly.

To support reuse of modules, a module implementation or header file can be templated.
Templating enables code generation which reduces boilerplate code.
FIXME: Exact details of templating is currently undefined.

When naming modules, the '.rtos-' substring is reserved for the RTOS module of a system.
A system may only have at most one RTOS module, and the **prj** tool makes the values of all of the configuration parameters of the RTOS module accessible to all other modules in the system, under a top-level dict entry named 'rtos'.
Consequently when naming top-level configuration parameters for modules, the name 'rtos' is reserved for this purpose.


**Future:** Modules that support multiple inclusion in a system vs. single inclusion in a system.


### Package

A *package* is a set of related modules and systems.
A package is simply a directory on the filesystem that contains modules or system.
For example, when creating a specific module `foo`, it may also have related modules such as `foo/mock`, which provides an alternative implementation that can be used by modules that need `foo` functionality, or `foo/test` which is used to test module `foo`.
Additionally, the `foo` module may have a `foo/test-system` that is system that can be run to test the functionality of the `foo` module.
Referencing the top-level of a package is the equivalent of importing `foo/entity`.
This is similar to the way in which Python packages import "package/__init__.py" when importing package.

**Future**: Not convinced that `entity` is the best name. This may change to `foo/module`, or perhaps `foo/foo`

### Device

The *device* is some target hardware that a system is designed to run on.


### Firmware Image

A firmware image is produced by the **prj** tool, and is something that can be subsequently installed and execute on a target device.


### Builder

A builder is a module that specifies how to build a system image from a set of source modules.
The project can have multiple specified builders, but each system must have exactly one builder (although the builder may be parameterised).
Just likes source modules, builders can also be configured for use in the system; simple examples would be compiler optimisation or warning flags.
A builder module is a Python script that exports a single public function `system_build`.
The `system_build` function should take a single `system` argument.
The `system_build` function should do perform the actual actions required to build the system (e.g.: running the compiler and linker).
When running external commands, the `execute` method (exported by `prj`) should be used.
The `system` object has an `output_file` attribute.
This is the path to the final system image.
`system_build` should ensure that this file is correctly generated (or raise a `SystemBuildError` exception).
The `system` object has an `output` attribute, which is a path to a directory where intermediate files can reside.
In the case where the `system_build` function requires creating intermediate files, they should only reside under the `system.output` directory.


Operations
------------

### Build

The *build* operation takes a system and generates a firmware image.
The command line must specify the system being built; where this is a variant system, the base system and all parameters must be specified.
The project definition specifies where output is generated to.
