#!/usr/bin/env python3.3
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

from pylib.tasks import new_review, new_task, tasks, integrate, _gen_tag
from pylib.tests import prj_test, x_test, pystache_test, rtos_test, check_pep8
from pylib.components import Component, build
from pylib.release import release_test, build_release, build_partials
from pylib.prj import prj_build
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
                       "armv7m": ["acamar", "gatria", "kraz", "acrux", "rigel"],
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
                            Component('blocking-mutex'),
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
               Component('task', {'task_start_api': False}),
               ],
    'gatria': [Component('reentrant'),
               Component('stack', pkg_component=True),
               Component('context-switch', pkg_component=True),
               Component('preempt-null'),
               Component('sched-rr', {'assume_runnable': True}),
               Component('simple-mutex'),
               Component('error'),
               Component('task', {'task_start_api': False}),
               Component('gatria'),
               ],
    'kraz': [Component('reentrant'),
             Component('stack', pkg_component=True),
             Component('context-switch', pkg_component=True),
             Component('preempt-null'),
             Component('sched-rr', {'assume_runnable': True}),
             Component('signal', {'yield_api': False}),
             Component('simple-mutex'),
             Component('error'),
             Component('task', {'task_start_api': False}),
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
              Component('task', {'task_start_api': False}),
              Component('acrux'),
              ],
    'rigel': [Component('reentrant'),
              Component('stack', pkg_component=True),
              Component('context-switch', pkg_component=True),
              Component('preempt-null'),
              Component('sched-rr', {'assume_runnable': False}),
              Component('signal', {'yield_api': True}),
              Component('timer', pkg_component=True),
              Component('timer'),
              Component('interrupt-event', pkg_component=True),
              Component('interrupt-event', {'timer_process': True}),
              Component('interrupt-event-signal', {'task_set': True}),
              Component('blocking-mutex'),
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
               Component('signal', {'yield_api': False}),
               Component('timer', pkg_component=True),
               Component('timer'),
               Component('interrupt-event', pkg_component=True),
               Component('interrupt-event', {'timer_process': True}),
               Component('interrupt-event-signal', {'task_set': False}),
               Component('blocking-mutex'),
               Component('simple-semaphore', {'timeouts': True}),
               Component('error'),
               Component('task', {'task_start_api': False}),
               Component('kochab'),
               ]
}

# client repositories may extend or override the following variables to control which configurations are available
skeletons = CORE_SKELETONS.copy()
configurations = CORE_CONFIGURATIONS.copy()


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    SUBCOMMAND_TABLE = {
        # Releases
        'prj-build': prj_build,
        'generate': build,
        'build-release': build_release,
        'build-partials': build_partials,
        'build-manuals': build_manuals,

        # Testing
        'check-pep8': check_pep8,
        'prj-test': prj_test,
        'pystache-test': pystache_test,
        'x-test': x_test,
        'rtos-test': rtos_test,
        'test-release': release_test,

        # Tasks management
        'review': new_review,
        'new': new_task,
        'list': tasks,
        'integrate': integrate,
        # Tempalte management
        'gen-tag': _gen_tag,
    }

    # create the top-level parser
    parser = argparse.ArgumentParser(prog='x.py')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    test_parser = subparsers.add_parser("test", help="Run tests")
    test_subparsers = test_parser.add_subparsers(title="Test suites", dest="test_command")

    _parser = test_subparsers.add_parser('check-pep8', help='Run PEP8 on project Python files')
    _parser.add_argument('--teamcity', action='store_true',
                         help="Provide teamcity output for tests",
                         default=False)
    _parser.add_argument('--excludes', nargs='*',
                         help="Exclude directories from pep8 checks",
                         default=[])
    for component_name in ['prj', 'x', 'rtos']:
        _parser = test_subparsers.add_parser(component_name + '-test', help='Run {} unittests'.format(component_name))
        _parser.add_argument('tests', metavar='TEST', nargs='*',
                             help="Specific test", default=[])
        _parser.add_argument('--list', action='store_true',
                             help="List tests (don't execute)",
                             default=False)
        _parser.add_argument('--verbose', action='store_true',
                             help="Verbose output",
                             default=False)
        _parser.add_argument('--quiet', action='store_true',
                             help="Less output",
                             default=False)
    test_subparsers.add_parser('pystache-test', help='Test pystache')
    test_subparsers.add_parser('test-release', help='Test final release')

    build_parser = subparsers.add_parser("build", help="Build release stuff...")
    build_subparsers = build_parser.add_subparsers(title="Build options", dest="build_command")

    build_subparsers.add_parser('prj-build', help='Build prj')
    build_subparsers.add_parser('build-release', help='Build final release')
    build_subparsers.add_parser('build-partials', help='Build partial release files')
    _parser = build_subparsers.add_parser('build-manuals', help='Build PDF manuals')
    _parser.add_argument('--verbose', '-v', action='store_true')
    build_subparsers.add_parser('generate', help='Generate packages from components')

    task_parser = subparsers.add_parser("task", help="Task management")
    task_subparsers = task_parser.add_subparsers(title="Task management operations", dest="task_command")

    task_subparsers.add_parser('list', help="List tasks")
    _parser = task_subparsers.add_parser('new', help='Create a new task')
    _parser.add_argument('taskname', metavar='TASKNAME', help='Name of the new task')
    _parser.add_argument('--no-fetch', dest='fetch', action='store_false', default='true', help='Disable fetchign')
    _parser = task_subparsers.add_parser('review', help='Create a new review')
    _parser.add_argument('reviewers', metavar='REVIEWER', nargs='+',
                         help='Username of reviewer')
    _parser = task_subparsers.add_parser('integrate', help='Integrate a completed development task/branch \
into the main upstream branch.')
    _parser.add_argument('--repo', help='Path of git repository to operate in. \
Defaults to current working directory.')
    _parser.add_argument('--name', help='Name of the task branch to integrate. \
Defaults to active branch in repository.')
    _parser.add_argument('--target', help='Name of branch to integrate task branch into. \
Defaults to "development".', default='development')
    _parser.add_argument('--archive', help='Prefix to add to task branch name when archiving it. \
Defaults to "archive".', default='archive')

    subparsers.add_parser('gen-tag', help='Generate a random 6-char alphanumeric string')

    args = parser.parse_args()

    if args.command is None:
        parser.print_help()
    else:
        for cmd, subcommand in ([("test", "test_command"), ("task", "task_command"), ("build", "build_command")]):
            if args.command == cmd:
                if vars(args)[subcommand] is None:
                    args = parser.parse_args([cmd, "-h"])
                args.command = vars(args)[subcommand]

        args.topdir = topdir
        args.configurations = configurations
        args.skeletons = skeletons

        return SUBCOMMAND_TABLE[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
