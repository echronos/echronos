#!/usr/bin/env python3.3
#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that no right, title
# or interest in or to any trade mark, service mark, logo or trade name
# of NICTA or its licensors is granted.
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
The enduser will need to install Python 3.3.
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
import sys
import os

externals = ['pep8', 'nose', 'ice']

# Make pylib available
sys.path.append(os.path.dirname(__file__))

from pylib.utils import BASE_DIR

sys.path = [os.path.join(BASE_DIR, 'external_tools', e) for e in externals] + sys.path
sys.path.insert(0, os.path.join(BASE_DIR, 'prj/app/pystache'))
if __name__ == '__main__':
    sys.modules['x'] = sys.modules['__main__']

### Check that the correct Python is being used.
correct = None
if sys.platform == 'darwin':
    correct = os.path.abspath(os.path.join(BASE_DIR, 'tools/x86_64-apple-darwin/bin/python3.3'))
elif sys.platform.startswith('linux'):
    correct = os.path.abspath(os.path.join(BASE_DIR, 'tools/x86_64-unknown-linux-gnu/bin/python3.3'))

if correct is not None and sys.executable != correct:
    print("x.py expects to be executed using {} (not {}).".format(correct, sys.executable), file=sys.stderr)
    sys.exit(1)

import argparse
import logging

from pylib.components import Component
from pylib import release, components, prj, tests, tasks
from pylib.manuals import build_manuals

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


CORE_CONFIGURATIONS = {"posix": ["sched-rr-test", "sched-prio-inherit-test", "simple-mutex-test",
                                 "blocking-mutex-test", "simple-semaphore-test", "sched-prio-test",
                                 "acamar", "gatria", "kraz"],
                       "armv7m": ["acamar", "gatria", "kraz", "acrux", "rigel", "kochab", "phact"],
                       "ppce500": ["acamar", "gatria", "kraz", "acrux", "kochab"]}

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
    'rigel': [Component('reentrant'),
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
    'kochab': [Component('reentrant'),
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
    'phact': [Component('reentrant'),
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
    COMMAND_TABLE = {
        'build': {
            'prj': prj.build,
            'packages': components.build,
            'release': release.build,
            'partials': release.build_partials,
            'docs': build_manuals,
        },
        'test': {
            'style': tests.style,
            'prj': tests.prj,
            'pystache': tests.pystache,
            'x': tests.x,
            'units': tests.units,
            'release': release.test,
            'licenses': tests.licenses,
            'provenance': tests.provenance,
            'systems': tests.systems,
        },
        'task': {
            'review': tasks.review,
            'create': tasks.create,
            'list': tasks.list,
            'integrate': tasks.integrate,
            'tag': tasks.tag,
        },
    }

    parser = argparse.ArgumentParser(prog='x.py')
    command_parsers = parser.add_subparsers(title='subcommands', dest='command')

    test_parsers = command_parsers.add_parser("test").add_subparsers(title="Test suites", dest="subcommand")

    p = test_parsers.add_parser('style', help='Run PEP8 on project Python files')
    p.add_argument('--teamcity', action='store_true', help="Provide teamcity output for tests", default=False)
    p.add_argument('--excludes', nargs='*', help="Exclude directories from pep8 checks", default=[])
    for component_name in ['prj', 'x', 'units']:
        p = test_parsers.add_parser(component_name)
        p.add_argument('tests', metavar='TEST', nargs='*', default=[])
        p.add_argument('--list', action='store_true', help="List tests (don't execute)", default=False)
        p.add_argument('--verbose', action='store_true', default=False)
        p.add_argument('--quiet', action='store_true', default=False)
    test_parsers.add_parser('pystache')
    test_parsers.add_parser('release')
    p = test_parsers.add_parser('licenses', help='Check that all files have the appropriate license header')
    p.add_argument('--excludes', nargs='*', help="Exclude directories from license header checks", default=[])
    test_parsers.add_parser('systems', help='Run system tests, i.e., tests that check the behavior of full \
RTOS systems. This command supports the same options as the Python nose test framework.')

    test_parsers.add_parser('provenance', help='Check that all files belonging to external tools map 1-1 with '
                                               'provenance listings')

    build_parsers = command_parsers.add_parser("build").add_subparsers(title="Build options", dest="subcommand")

    build_parsers.add_parser('prj')
    build_parsers.add_parser('release')
    p = build_parsers.add_parser('partials', help='Build partial release files')
    p.add_argument('--allow-unknown-filetypes', action='store_true')
    p = build_parsers.add_parser('docs')
    p.add_argument('--verbose', '-v', action='store_true')
    build_parsers.add_parser('packages', help='Generate packages from components')

    task_parsers = command_parsers.add_parser("task").add_subparsers(title="Task management operations",
                                                                     dest="subcommand")

    task_parsers.add_parser('list')
    p = task_parsers.add_parser('create')
    p.add_argument('taskname', metavar='TASKNAME')
    p.add_argument('--no-fetch', dest='fetch', action='store_false', default='true')
    p = task_parsers.add_parser('review')
    p.add_argument('reviewers', metavar='REVIEWER', nargs='+')
    p = task_parsers.add_parser('integrate', help='Integrate a completed development task/branch \
into the main upstream branch.')
    p.add_argument('--repo', help='Path of git repository to operate in. \
Defaults to current working directory.')
    p.add_argument('--name', help='Name of the task branch to integrate. \
Defaults to active branch in repository.')
    p.add_argument('--target', help='Name of branch to integrate task branch into. \
Defaults to "development".', default='development')
    p.add_argument('--archive', help='Prefix to add to task branch name when archiving it. \
Defaults to "archive".', default='archive')
    task_parsers.add_parser('tag', help='Generate a random 6-char alphanumeric string')

    # parse arbitrary nose options for the 'test systems' command
    # argparse does not seem to provide a better mechanism for this case
    args, unknown_args = parser.parse_known_args()
    if args.command == 'test' and args.subcommand == 'systems':
        args.unknown_args = unknown_args
    else:
        # enforce stricter parsing for other commands
        args = parser.parse_args()

    if args.command not in COMMAND_TABLE or args.subcommand not in COMMAND_TABLE[args.command]:
        # argparse does not support required subparsers so it does not itself reject a command line that lacks a
        # command or subcommand
        parser.print_help()
    else:
        args.topdir = topdir
        args.configurations = configurations
        args.skeletons = skeletons

        return COMMAND_TABLE[args.command][args.subcommand](args)


if __name__ == "__main__":
    sys.exit(main())
