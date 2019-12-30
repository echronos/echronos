#!/usr/bin/env python3
#
# eChronos Real-Time Operating System
# Copyright (c) 2017, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#

"""
Overview
---------
`x.py` is the main *project management script* for the RTOS project.
As a *project magement script* it should handle any actions related to working on the project, such as building
artifacts for release, project management related tasks such as creating reviews, and similar task.
Any project management related task should be added as a subcommand to `x.py`, rather than adding another script.

Released Files
---------------

One of the main tasks of `x.py` is to create the releasable artifacts (i.e.: things that will be shipped to users).

### `prj` release information

prj will be distributed in source format for now as the customer likes it that way, and also because of the
impracticality of embedding python3 into a distributable .exe .
The enduser will need to install Python 3.
The tool can be embedded (not installed) into a project tree (i.e.: used inplace).

### Package release information

Numerous *packages* will be release to the end user.

One or more RTOS packages will be released.
These include:
* The RTOS core
* The RTOS optional-modules
* Test Suite
* Documentation (PDF)

Additionally one or more build modules will be released.
These include:
* Module
* Documentation

### Built files

The following output files will be produced by `x.py`.

* release/prj-<version>.zip
* release/<rtos-foo>-<version>.zip
* release/<build-name>-<version>.zip

Additional Requirements
-----------------------

This `x.py` tool shouldn't leave old files around (like every other build tool on the planet.)
So, if `x.py` is building version 3 of a given release, it should ensure old releases are not still in the `releases`
directory.

"""
import argparse
import logging
import os
import sys

# Make pylib available for importing - this is necessary for x.py wrappers in client repositories to work
sys.path.append(os.path.dirname(__file__))

# pylint: disable=wrong-import-position
from pylib.utils import BASE_DIR
sys.path = [os.path.join(BASE_DIR, 'external_tools')] + sys.path
sys.path.insert(0, os.path.join(BASE_DIR, 'prj/app/pystache'))
if __name__ == '__main__':
    sys.modules['x'] = sys.modules['__main__']

from pylib.components import Component
from pylib import release, components, prj, tests, docs  # pylint: disable=unused-import
from pylib.cmdline import add_subcommands_to_parser

# Set up a specific logger with our desired output level
logger = logging.getLogger()  # pylint: disable=invalid-name
logger.setLevel(logging.INFO)


# topdir is the rtos repository directory in which the user invoked the x tool.
# If the x tool is invoked from a client repository through a wrapper, topdir contains the directory of that client
# repository.
# If the user directly invokes x tool of the RTOS core, topdir is the directory of this file.
# topdir defaults to the core directory.
# It may be modified by an appropriate invocation of main().
topdir = os.path.normpath(os.path.dirname(__file__))  # pylint: disable=invalid-name


# The POSIX context switch component depends on ucontext.h which is supported on real POSIX platforms,
# on cygwin, but not on MinGW
CORE_CONFIGURATIONS = {
                       "riscv": ["rigel"]}

CORE_SKELETONS = {
    
    'rigel': [Component('docs'),
              Component('error'),
              # Please note that the task_start_api pystache tag is used solely to block out a rigel-specific section
              # of the Task Configuration chapter.
              Component('api-conditions'),
              Component('task', {'task_start_api': True, 'scheduler': True, 'has_interrupts': True}),
              Component('rigel')],
    
}

# client repositories may extend or override the following variables to control which configurations are available
skeletons = CORE_SKELETONS.copy()  # pylint: disable=invalid-name
configurations = CORE_CONFIGURATIONS.copy()  # pylint: disable=invalid-name


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    parser = argparse.ArgumentParser(prog='x.py')
    add_subcommands_to_parser(globals(), parser)

    # parse arbitrary options for the 'test systems' command
    # argparse does not seem to provide a better mechanism for this case

    args, unknown_args = parser.parse_known_args()
    if args.command == 'test' and args.subcommand == 'systems':
        args.unknown_args = unknown_args
    else:
        # enforce stricter parsing for other commands
        args = parser.parse_args()

    if not args.command or not args.subcommand:
        # argparse does not support required subparsers so it does not itself reject a command line that lacks a
        # command or subcommand
        parser.print_help()
        return 1

    args.topdir = topdir
    args.configurations = configurations
    args.skeletons = skeletons

    return args.execute(args)


if __name__ == "__main__":
    RESULT = main()
    # sys.exit(None) makes the process exit with exit code 0, which indicates successful completion.
    # In the past, e.g. test functions have returned None, even when there were test errors.
    # To prevent this, require the functions called by main() to consistently return an integer value.
    if isinstance(RESULT, int):
        sys.exit(RESULT)
    else:
        raise TypeError('The main() function shall return an integer, but returned a value of type {} instead.'
                        .format(type(RESULT)))
