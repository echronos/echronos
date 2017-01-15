#!/usr/bin/env python3
#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that these additional
# terms apply under section 7:
#
#   No right, title or interest in or to any trade mark, service mark, logo or
#   trade name of of National ICT Australia Limited, ABN 62 102 206 173
#   ("NICTA") or its licensors is granted. Modified versions of the Program
#   must be plainly marked as such, and must not be distributed using
#   "eChronos" as a trade mark or product name, or misrepresented as being the
#   original Program.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
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

externals = ['nose', '']

# Make pylib available for importing - this is necessary for x.py wrappers in client repositories to work
sys.path.append(os.path.dirname(__file__))

from pylib.utils import BASE_DIR
sys.path = [os.path.join(BASE_DIR, 'external_tools', e) for e in externals] + sys.path
sys.path.insert(0, os.path.join(BASE_DIR, 'prj/app/pystache'))
if __name__ == '__main__':
    sys.modules['x'] = sys.modules['__main__']

from pylib.components import Component
from pylib import release, components, prj, tests, docs  # pylint: disable=unused-import
from pylib.cmdline import add_subcommands_to_parser

# Set up a specific logger with our desired output level
logger = logging.getLogger()
logger.setLevel(logging.INFO)


# topdir is the rtos repository directory in which the user invoked the x tool.
# If the x tool is invoked from a client repository through a wrapper, topdir contains the directory of that client
# repository.
# If the user directly invokes x tool of the RTOS core, topdir is the directory of this file.
# topdir defaults to the core directory.
# It may be modified by an appropriate invocation of main().
topdir = os.path.normpath(os.path.dirname(__file__))


# The POSIX context switch component depends on ucontext.h which is supported on real POSIX platforms,
# on cygwin, but not on MinGW
CORE_CONFIGURATIONS = {"posix": ["sched-rr-test", "sched-prio-inherit-test", "simple-mutex-test",
                                 "blocking-mutex-test", "simple-semaphore-test", "sched-prio-test",
                                 "acamar", "gatria", "kraz", "acrux", "rigel"],
                       "armv7m": ["acamar", "gatria", "kraz", "acrux", "rigel", "kochab", "phact"],
                       "ppce500": ["acamar", "gatria", "kraz", "acrux", "kochab", "phact"],
                       "stub": ["acamar", "gatria", "kraz", "acrux", "rigel", "kochab", "phact"]}

CORE_SKELETONS = {
    'sched-rr-test': [Component('reentrant'),
                      Component('sched-rr', {'assume_runnable': False}),
                      Component('sched-rr-test'),
                      ],
    'sched-prio-test': [Component('reentrant'),
                        Component('sched-prio', {'assume_runnable': False}),
                        Component('sched-prio-test'),
                        ],
    'sched-prio-inherit-test': [Component('reentrant'),
                                Component('sched-prio-inherit', {'assume_runnable': False}),
                                Component('sched-prio-inherit-test'),
                                ],
    'simple-mutex-test': [Component('reentrant'),
                          Component('simple-mutex'),
                          Component('simple-mutex-test'),
                          ],
    'blocking-mutex-test': [Component('reentrant'),
                            Component('blocking-mutex', {'lock_timeout': False, 'prio_ceiling': False}),
                            Component('blocking-mutex-test'),
                            ],
    'simple-semaphore-test': [Component('reentrant'),
                              Component('preempt-null'),
                              Component('simple-semaphore', {'timeouts': False}),
                              Component('simple-semaphore-test'),
                              ],
    'acamar': [Component('reentrant'),
               Component('acamar'),
               Component('stack', pkg_component=True),
               Component('context-switch', pkg_component=True),
               Component('error'),
               Component('task'),
               ],
    'gatria': [Component('reentrant'),
               Component('stack', pkg_component=True),
               Component('context-switch', pkg_component=True),
               Component('preempt-null'),
               Component('sched-rr', {'assume_runnable': True}),
               Component('simple-mutex'),
               Component('error'),
               Component('task'),
               Component('gatria'),
               ],
    'kraz': [Component('reentrant'),
             Component('stack', pkg_component=True),
             Component('context-switch', pkg_component=True),
             Component('preempt-null'),
             Component('sched-rr', {'assume_runnable': True}),
             Component('signal', {'prio_inherit': False}),
             Component('simple-mutex'),
             Component('error'),
             Component('task'),
             Component('kraz'),
             ],
    'acrux': [Component('reentrant'),
              Component('stack', pkg_component=True),
              Component('context-switch', pkg_component=True),
              Component('preempt-null'),
              Component('sched-rr', {'assume_runnable': False}),
              Component('interrupt-event', pkg_component=True),
              Component('interrupt-event', {'timer_process': False}),
              Component('simple-mutex'),
              Component('error'),
              Component('task'),
              Component('acrux'),
              ],
    'rigel': [Component('docs'),
              Component('reentrant'),
              Component('stack', pkg_component=True),
              Component('context-switch', pkg_component=True),
              Component('preempt-null'),
              Component('sched-rr', {'assume_runnable': False}),
              Component('signal', {'prio_inherit': False, 'yield_api': True, 'task_signals': True}),
              Component('timer', pkg_component=True),
              Component('timer', {'preemptive': False}),
              Component('interrupt-event', pkg_component=True),
              Component('interrupt-event', {'timer_process': True}),
              Component('interrupt-event-signal', {'task_set': True}),
              Component('blocking-mutex', {'lock_timeout': False, 'preemptive': False, 'prio_ceiling': False}),
              Component('profiling'),
              Component('message-queue'),
              Component('error'),
              # Please note that the task_start_api pystache tag is used solely to block out a rigel-specific section
              # of the Task Configuration chapter.
              Component('task', {'task_start_api': True}),
              Component('rigel'),
              ],
    'kochab': [Component('docs'),
               Component('reentrant'),
               Component('stack', pkg_component=True),
               Component('context-switch-preempt', pkg_component=True),
               Component('sched-prio-inherit', {'assume_runnable': False}),
               Component('signal', {'prio_inherit': True, 'yield_api': False, 'task_signals': False}),
               Component('timer', pkg_component=True),
               Component('timer', {'preemptive': True}),
               Component('interrupt-event', pkg_component=True),
               Component('interrupt-event', {'timer_process': True}),
               Component('interrupt-event-signal', {'task_set': False}),
               Component('blocking-mutex', {'lock_timeout': True, 'preemptive': True, 'prio_ceiling': False}),
               Component('simple-semaphore', {'timeouts': True, 'preemptive': True}),
               Component('error'),
               Component('task', {'task_start_api': False}),
               Component('kochab'),
               ],
    'phact': [Component('docs'),
              Component('reentrant'),
              Component('stack', pkg_component=True),
              Component('context-switch-preempt', pkg_component=True),
              Component('sched-prio-ceiling', {'assume_runnable': False}),
              Component('signal', {'prio_inherit': False, 'yield_api': False, 'task_signals': False}),
              Component('timer', pkg_component=True),
              Component('timer', {'preemptive': True}),
              Component('interrupt-event', pkg_component=True),
              Component('interrupt-event', {'timer_process': True}),
              Component('interrupt-event-signal', {'task_set': False}),
              Component('blocking-mutex', {'lock_timeout': True, 'preemptive': True, 'prio_ceiling': True}),
              Component('simple-semaphore', {'timeouts': True, 'preemptive': True}),
              Component('error'),
              Component('task', {'task_start_api': False}),
              Component('phact'),
              ],
}

# client repositories may extend or override the following variables to control which configurations are available
skeletons = CORE_SKELETONS.copy()
configurations = CORE_CONFIGURATIONS.copy()


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    parser = argparse.ArgumentParser(prog='x.py')
    add_subcommands_to_parser(globals(), parser)

    # parse arbitrary nose options for the 'test systems' command
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
    else:
        args.topdir = topdir
        args.configurations = configurations
        args.skeletons = skeletons

        return args.execute(args)


if __name__ == "__main__":
    result = main()
    # sys.exit(None) makes the process exit with exit code 0, which indicates successful completion.
    # In the past, e.g. test functions have returned None, even when there were test errors.
    # To prevent this, require the functions called by main() to consistently return an integer value.
    if isinstance(result, int):
        sys.exit(result)
    else:
        raise TypeError('The main() function shall return an integer, but returned a value of type {} instead.'
                        .format(type(result)))
