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
from pylib.components import Component, ArchitectureComponent, Architecture, RtosSkeleton, build, generate_rtos_module
from pylib.release import release_test, build_release, build_partials
from pylib.prj import prj_build
from pylib.manuals import build_manuals
from pylib.utils import get_executable_extension, get_host_platform_name

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


def get_platform_tools_dir():
    return os.path.join(BASE_DIR, 'tools', get_host_platform_name())


class ExecutableNotAvailable(Exception):
    pass


def get_executable_from_repo_or_system(name):
    """Get the path of an executable, searching first in the repository's tool directory, then on the system.

    `name` is the base name of the executable without path components and without extensions.

    If no executable with the given name can be found, an ExecutableNotAvailable exception is raised.

    Example:
    - On Windows:
      get_executable_from_repo_or_system('pandoc') => '.\\tools\\win32\\bin\\pandoc.exe'
    - On Linux with pandoc installed in the system:
      get_executable_from_repo_or_system('pandoc') => '/usr/bin/pandoc'

    """
    path = os.path.join(get_platform_tools_dir(), 'bin', name + get_executable_extension())
    if not os.path.exists(path):
        path = shutil.which(name)
        if not path:
            raise ExecutableNotAvailable('Unable to find the executable "{}" in the repository or the system. \
This may be resolved by installing the executable on your system or it may indicate that the RTOS toolchain does not \
support your host platform.'.format(name))
    return path


def get_package_dirs(required_files=None):
    if required_files is None:
        required_files = set()

    for root, dirs, files in os.walk(os.path.join(BASE_DIR, 'packages')):
        if required_files.issubset(files):
            yield root


def get_doc_vars(markdown_file):
    doc_vars = {}
    for line in open(markdown_file).readlines():
        if line.startswith('<!-- %'):
            key, value = line.strip()[6:-4].split(' ', 1)
            doc_vars[key] = value
    return doc_vars


def build_manual(pkg_dir, verbose=False):
    markdown_file = os.path.join(pkg_dir, 'documentation.markdown')
    pdf_file = os.path.join(pkg_dir, 'documentation.pdf')
    html_file = os.path.join(pkg_dir, 'documentation.html')

    pandoc_executable = get_executable_from_repo_or_system('pandoc')
    doc_vars = get_doc_vars(markdown_file)
    pandoc_vars = ' '.join(['-V{}="{}"'.format(key, value) for key, value in doc_vars.items()])
    pandoc_cmd = '{}\
                  --write html\
                  --standalone\
                  --template="{}"\
                  --css="documentation_stylesheet.css"\
                  --toc --toc-depth=2\
                  {}\
                  --output="{}"\
                  "{}"'
    pandoc_cmd = pandoc_cmd.format(pandoc_executable,
                                   # pandoc fails if the template path is relative, so make it absolute:
                                   os.path.abspath(os.path.join(pkg_dir, 'documentation_template.html')),
                                   pandoc_vars,
                                   html_file,
                                   markdown_file)
    if verbose:
        print(pandoc_cmd)
    subprocess.check_call(pandoc_cmd)

    wkh_executable = get_executable_from_repo_or_system('wkhtmltopdf')
    wkh_cmd = '{}\
               --outline-depth 2\
               --page-size A4\
               --margin-top 20\
               --margin-bottom 25\
               --margin-left 20\
               --margin-right 20\
               --header-spacing 5\
               --header-html "{}"\
               --footer-spacing 5\
               --footer-html "{}"\
               --replace docid "Document ID: {}"\
               "{}" "{}"'
    wkh_cmd = wkh_cmd.format(wkh_executable,
                             os.path.join(pkg_dir, 'documentation_header.html'),
                             os.path.join(pkg_dir, 'documentation_footer.html'),
                             doc_vars['docid'],
                             html_file,
                             pdf_file)
    if verbose:
        print(wkh_cmd)
    subprocess.check_call(wkh_cmd)


def build_manuals(args):
    build([])
    for pkg_dir in get_package_dirs(set(('documentation.markdown',))):
        build_manual(pkg_dir, args.verbose)


class OverrideFunctor:
    def __init__(self, function, *args, **kwargs):
        self.function = function
        self.args = args
        self.kwargs = kwargs

    def __call__(self, *args, **kwargs):
        return self.function(*self.args, **self.kwargs)


CORE_ARCHITECTURES = {
    'posix': Architecture('posix', {}),
    'armv7m': Architecture('armv7m', {}),
    'ppce500': Architecture('ppce500', {}),
}

CORE_SKELETONS = {
    'sched-rr-test': RtosSkeleton(
        'sched-rr-test',
        [Component('reentrant'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('sched-rr-test')]),
    'sched-prio-test': RtosSkeleton(
        'sched-prio-test',
        [Component('reentrant'),
         Component('sched', 'sched-prio', {'assume_runnable': False}),
         Component('sched-prio-test')]),
    'sched-prio-inherit-test': RtosSkeleton(
        'sched-prio-inherit-test',
        [Component('reentrant'),
         Component('sched', 'sched-prio-inherit', {'assume_runnable': False}),
         Component('sched-prio-inherit-test')]),
    'simple-mutex-test': RtosSkeleton(
        'simple-mutex-test',
        [Component('reentrant'),
         Component('mutex', 'simple-mutex'),
         Component('simple-mutex-test')]),
    'blocking-mutex-test': RtosSkeleton(
        'blocking-mutex-test',
        [Component('reentrant'),
         Component('mutex', 'blocking-mutex'),
         Component('blocking-mutex-test')]),
    'simple-semaphore-test': RtosSkeleton(
        'simple-semaphore-test',
        [Component('reentrant'),
         Component('semaphore', 'simple-semaphore'),
         Component('simple-semaphore-test')]),
    'acamar': RtosSkeleton(
        'acamar',
        [Component('reentrant'),
         Component('acamar'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('error'),
         Component('task'),
         ]),
    'gatria': RtosSkeleton(
        'gatria',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('gatria'),
         ]),
    'kraz': RtosSkeleton(
        'kraz',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('signal'),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('kraz'),
         ]),
    'acrux': RtosSkeleton(
        'acrux',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         ArchitectureComponent('interrupt_event_arch', 'interrupt-event'),
         Component('interrupt_event', 'interrupt-event', {'timer_process': False, 'task_set': False}),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('acrux'),
         ]),
    'rigel': RtosSkeleton(
        'rigel',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('signal'),
         ArchitectureComponent('timer_arch', 'timer'),
         Component('timer'),
         ArchitectureComponent('interrupt_event_arch', 'interrupt-event'),
         Component('interrupt_event', 'interrupt-event', {'timer_process': True, 'task_set': True}),
         Component('mutex', 'blocking-mutex'),
         Component('profiling'),
         Component('message-queue'),
         Component('error'),
         Component('task'),
         Component('rigel'),
         ],
    ),
    # This is a preliminary, incomplete version of kochab without yet interrupts or preemption
    'kochab': RtosSkeleton(
        'kochab',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-prio-inherit', {'assume_runnable': False}),
         Component('signal'),
         Component('mutex', 'blocking-mutex'),
         Component('semaphore', 'simple-semaphore'),
         Component('error'),
         Component('task'),
         Component('kochab'),
         ]),
}


CORE_CONFIGURATIONS = {
    'sched-rr-test': ['posix'],
    'sched-prio-inherit-test': ['posix'],
    'simple-mutex-test': ['posix'],
    'blocking-mutex-test': ['posix'],
    'simple-semaphore-test': ['posix'],
    'sched-prio-test': ['posix'],
    'acamar': ['posix', 'armv7m', 'ppce500'],
    'gatria': ['posix', 'armv7m', 'ppce500'],
    'kraz': ['posix', 'armv7m', 'ppce500'],
    'acrux': ['armv7m', 'ppce500'],
    'rigel': ['armv7m'],
    'kochab': ['ppce500'],
}


# client repositories may extend or override the following variables to control which configurations are available
architectures = CORE_ARCHITECTURES.copy()
skeletons = CORE_SKELETONS.copy()
configurations = CORE_CONFIGURATIONS.copy()


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    SUBCOMMAND_TABLE = {
        # prj tool
        'prj-build': prj_build,
        # Releases
        'build': build,
        'test-release': release_test,
        'build-release': build_release,
        'build-partials': build_partials,
        # Manuals
        'build-manuals': build_manuals,
        # Testing
        'check-pep8': check_pep8,
        'prj-test': prj_test,
        'pystache-test': pystache_test,
        'x-test': x_test,
        'rtos-test': rtos_test,
        # Tasks management
        'new-review': new_review,
        'new-task': new_task,
        'tasks': tasks,
        'integrate': integrate,
        # Tempalte management
        'gen-tag': _gen_tag,
    }

    # create the top-level parser
    parser = argparse.ArgumentParser(prog='x.py')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    # create the parser for the "prj.pep8" command
    subparsers.add_parser('tasks', help="List tasks")

    _parser = subparsers.add_parser('check-pep8', help='Run PEP8 on project Python files')
    _parser.add_argument('--teamcity', action='store_true',
                         help="Provide teamcity output for tests",
                         default=False)
    _parser.add_argument('--excludes', nargs='*',
                         help="Exclude directories from pep8 checks",
                         default=[])

    for component_name in ['prj', 'x', 'rtos']:
        _parser = subparsers.add_parser(component_name + '-test', help='Run {} unittests'.format(component_name))
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
    subparsers.add_parser('prj-build', help='Build prj')

    subparsers.add_parser('pystache-test', help='Test pystache')
    subparsers.add_parser('build-release', help='Build final release')
    subparsers.add_parser('test-release', help='Test final release')
    subparsers.add_parser('build-partials', help='Build partial release files')
    _parser = subparsers.add_parser('build-manuals', help='Build PDF manuals')
    _parser.add_argument('--verbose', '-v', action='store_true')
    subparsers.add_parser('build', help='Build all release files')
    _parser = subparsers.add_parser('new-review', help='Create a new review')
    _parser.add_argument('reviewers', metavar='REVIEWER', nargs='+',
                         help='Username of reviewer')

    _parser = subparsers.add_parser('new-task', help='Create a new task')
    _parser.add_argument('taskname', metavar='TASKNAME', help='Name of the new task')
    _parser.add_argument('--no-fetch', dest='fetch', action='store_false', default='true', help='Disable fetchign')

    # generate parsers and command table entries for generating RTOS variants
    for rtos_name, arch_names in configurations.items():
        SUBCOMMAND_TABLE[rtos_name + '-gen'] = OverrideFunctor(generate_rtos_module,
                                                               skeletons[rtos_name],
                                                               [architectures[arch] for arch in arch_names])
        subparsers.add_parser(rtos_name + '-gen', help="Generate {} RTOS".format(rtos_name))

    _parser = subparsers.add_parser('integrate', help='Integrate a completed development task/branch into the main \
upstream branch.')
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

    # Default to building
    if args.command is None:
        args.command = 'build'
    args.topdir = topdir
    args.configurations = configurations
    args.skeletons = skeletons
    args.architectures = architectures

    return SUBCOMMAND_TABLE[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
