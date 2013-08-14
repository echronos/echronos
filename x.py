#!/usr/bin/env python3.3
"""
Overview
---------
`x.py` is the main *project management script* for the brtos project.
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

# FIXME: Use correct declaration vs definition.
EXPECTED_SECTIONS = ['public_headers',
                     'public_type_definitions',
                     'public_structure_definitions',
                     'public_object_like_macros',
                     'public_function_like_macros',
                     'public_extern_definitions',
                     'public_function_definitions',
                     'headers',
                     'object_like_macros',
                     'type_definitions',
                     'structure_definitions',
                     'extern_definitions',
                     'function_definitions',
                     'state',
                     'function_like_macros',
                     'functions',
                     'public_functions']


def follow_link(l):
    """Return the underlying file from a symbolic link.

    This will (recursively) follow links until a non-symbolic link is found.
    No cycle checks are performed by this function.

    """
    if not os.path.islink(l):
        return l
    p = os.readlink(l)
    if os.path.isabs(p):
        return p
    return follow_link(os.path.join(os.path.dirname(l), p))


externals = ['pep8', 'nose', 'ice']
BASE_DIR = os.path.normpath(os.path.dirname(follow_link(__file__)))
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
import calendar
import collections
import datetime
import functools
import glob
import importlib
import inspect
import io
import ice
import logging
import nose
import pep8
import pystache
import re
import signal
import shutil
import string
import subprocess
import tarfile
import tempfile
import time
import unittest
import zipfile
from contextlib import contextmanager
from glob import glob
from random import choice
from nose.plugins.skip import SkipTest

# Set up a specific logger with our desired output level
logger = logging.getLogger()
logger.setLevel(logging.INFO)


BASE_TIME = calendar.timegm((2013, 1, 1, 0, 0, 0, 0, 0, 0))

IN_TEAMCITY = 'TEAMCITY_PROJECT_NAME' in os.environ


# topdir is the rtos repository directory in which the user invoked the x tool.
# If the x tool is invoked from a client repository through a wrapper, topdir contains the directory of that client
# repository.
# If the user directly invokes x tool of the RTOS core, topdir is the directory of this file.
# topdir defaults to the core directory.
# It may be modified by an appropriate invocation of main().
topdir = os.path.normpath(os.path.dirname(__file__))


def gen_tag():
    tag_length = 6
    tag_chars = string.ascii_letters + string.digits
    return ''.join(choice(tag_chars) for _ in range(tag_length))


SIG_NAMES = dict((k, v) for v, k in signal.__dict__.items() if v.startswith('SIG'))


def show_exit(exit_code):
    sig_num = exit_code & 0xff
    exit_status = exit_code >> 8
    if sig_num == 0:
        return "exit: {}".format(exit_status)
    else:
        return "signal: {}".format(SIG_NAMES.get(sig_num, 'Unknown signal {}'.format(sig_num)))


def sort_typedefs(typedef_lines):
    """Given a string containing multiple lines of typedefs, sort the lines so that the typedefs are in the 'correct'
    order.

    The typedef lines must only contain typedefs.
    No comments or other data is allowed.
    Blank lines are allowed, but will be ommited from the output.

    The correct order for typedefs is on such that if typedef 'b' is defined in terms of typedef 'a', typedef 'a' will
    appear first in the sorted output.

    """

    typedefs = []
    for l in typedef_lines.split('\n'):
        if l == '':
            continue
        if not l.endswith(';'):
            raise Exception("Expect a typedef line to end with ';' ({})".format(l))
        parts = l[:-1].split()
        if not parts[0] == 'typedef':
            raise Exception("Expect typedef line to startwith 'typedef'")
        new_type = parts[-1]
        old_type = ' '.join(parts[1:-1])
        typedefs.append((new_type, old_type))

    new_types = [new for (new, _) in typedefs]
    r = []

    # First put in any types that don't cross reference.
    #  we assume they are defined in other headers.
    for (new, old) in typedefs[:]:
        if old not in new_types:
            r.append((new, old))
            typedefs.remove((new, old))

    # Now, for each new type
    i = 0
    while i < len(r):
        check_type = r[i][0]
        i += 1
        for (new, old) in typedefs[:]:
            if old == check_type:
                r.append((new, old))
                typedefs.remove((new, old))

    return '\n'.join(['typedef {} {};'.format(old, new) for (new, old) in r])


@contextmanager
def chdir(path):
    """Current-working directory context manager.

    Makes the current working directory the specified `path` for the duration of the context.

    Example:

    with chdir("newdir"):
        # Do stuff in the new directory
        pass

    """
    cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(cwd)


@contextmanager
def tempdir():
    tmpdir = tempfile.mkdtemp()
    try:
        yield tmpdir
    finally:
        shutil.rmtree(tmpdir)


def walk(path, flt=None):
    """Return a list of all files in a given path. flt can be used to filter
    any unwanted files."""
    def always_true(x):
        return True

    if flt is None:
        flt = always_true

    file_list = []
    for root, dirs, files in os.walk(path):
        file_list.extend([os.path.join(root, f) for f in files if not flt(os.path.join(root, f))])
    return file_list


def base_path(*path):
    """Join one or more pathname components to the directory in which the
    script resides.

    The goal of this script is to easily allow pathnames that are relative
    to the directory in which the script resides.

    If the script is run as `./x.py` `base_path('foo')` will return
    ./foo.

    If the script is run by an absolute path (e.g.: `/path/to/x.py`)
    `base_path('foo')` will return `/path/to/foo`.

    If user is in the `./bar` directory and runs the script as
    `../x.py`, `base_path('foo')` will return `../bar`.

    The path returned by `base_path` will allow access to the file
    assuming that the current working directory has not been changed.
    """
    return os.path.join(BASE_DIR, *path)


def top_path(*path):
    """Return a path relative to the directory in which the x tool or wrapper was invoked.

    This function is equivalent to base_path(), except when the x tool is invoked in a client repository through a
    wrapper.
    In that case, the specified path is not appended to the directory containing the core x.py file, but the directory
    containing the wrapper x.py file invoked by the user.

    """
    return os.path.join(topdir, *path)


def base_to_top_paths(*path):
    """For each directory from BASE_DIR up to topdir in the directory tree, append the specified path and return the
    resulting sequence.

    For example, if topdir is '/rtos/', BASE_DIR is '/rtos/core/', and *path is ['packages'], this function returns
    ['/rtos/core/packages', '/rtos/packages']

    If topdir equals BASE_DIR, the result of this function is a sequence with a single element and equal to
    [base_path(*path)]

    """
    result = []

    cur_dir = os.path.abspath(BASE_DIR)
    stop_dir = os.path.abspath(topdir)
    iterate = True
    while iterate:
        result.append(os.path.join(cur_dir, *path))
        iterate = (cur_dir != stop_dir)
        cur_dir = os.path.dirname(cur_dir)

    return result


def un_base_path(path):
    """Reverse the operation performed by `base_path`.

    For all `x`, `un_base_path(base_path(x)) == x`.
    """
    if BASE_DIR == '':
        return path
    else:
        return path[len(BASE_DIR) + 1:]


def check_pep8(args):
    """Check for PEP8 compliance with the pep8 tool.

    This implements conventions lupHw1 and u1wSS9.
    The enforced maximum line length follows convention TZb0Uv.

    When the pep8 tool finds all project Python files to be compliant, this function returns None.
    When a non-compliant file is found, details about the non-compliance are printed on the standard output stream and
    this function returns 1.
    Runtime errors encountered by the pep8 tool are printed on the standard error stream and raised as the appropriate
    exceptions.

    """
    excludes = ['external_tools', 'pystache', 'tools', 'ply']
    exclude_patterns = ','.join(excludes)
    options = ['--exclude=' + exclude_patterns, '--max-line-length', '118', top_path('.')]

    logging.info('pep8 check: ' + ' '.join(options))

    try:
        pep8.pep8(options)
    except pep8.Pep8Error:
        logging.error('pep8 check found non-compliant files')  # details on stdout
        return 1


@contextmanager
def python_path(*paths):
    """A context manager that adds (and removes) one or more directories from the Python path.

    This allows extending the Python path temporarily to load certain modules.
    The directories are expected as individual arguments, e.g., "with python_path('foo', 'bar'):"

    """
    paths = [os.path.abspath(path) for path in paths]
    sys.path = paths + sys.path
    try:
        yield
    finally:
        del sys.path[:len(paths)]


class Component:
    """Represents an optional, exchangeable piece of functionality of an RTOS.

    Components reside in the components/ directory of the core or sub-projects.
    This class transparently finds components in any of the available core or sub-projects.
    Instances of this class encapsulate the act of parsing a component file and converting it into configuration data
    used when generating an RtosModule.

    """
    _search_paths = None

    @staticmethod
    def _get_search_paths():
        search_paths = []

        current_dir = BASE_DIR
        while True:
            components_dir = os.path.join(current_dir, 'components')
            if os.path.isdir(components_dir):
                search_paths.append(components_dir)
                next_dir = os.path.dirname(current_dir)
                if next_dir != current_dir:
                    current_dir = next_dir
                else:
                    break
            else:
                break

        # reverse the search paths so that they are sorted by increasing depth in the directory tree
        # this is expected to lead to components in client repositories to override those in the core repository
        search_paths.reverse()

        return search_paths

    @staticmethod
    def get_search_paths():
        """Find and return the directories that, by convention, are expected to contain component modules.

        As search directories qualify all directories called 'components' in the BASE_DIR or its parent directories.
        The search for such directories upwards in the directory tree from BASE_DIR stops at the first parent
        directory not containing a 'components' directory.

        """
        if Component._search_paths is None:
            Component._search_paths = Component._get_search_paths()
        return Component._search_paths

    @staticmethod
    def find(partial_path):
        """Find the component partial_path in the core repository or client repositories further up in the directory
        tree."""
        for search_path in Component.get_search_paths():
            component_path = os.path.join(search_path, partial_path)
            if os.path.exists(component_path):
                return component_path
        raise KeyError('Unable to find component "{}"'.format(partial_path))

    def __init__(self, name, resource_name=None, configuration={}):
        """Create a component object.

        Such objects encapsulate the act of parsing a corresponding source file.
        The parsed data is converted into configuration information used when generating an RtosModule by rendering an
        RTOS template file.

        'name' is the component name used in the RTOS template file.
        For example, the properties of the irq event component are referred to as 'irq_event.xyz' in the
        RTOS template files.

        'resource_name' is the base name of the source file of this component that is parsed to obtain this
        component's properties.
        For example, the base name of the irq event component is 'irq-event', which expands to the on-disk
        file name of irq-event.c.

        'configuration' is a dictionary with configuration information.
        It is passed to the 'parse_sectioned_file()' function used to parse this component's source file.

        """
        self.name = name
        if resource_name is not None:
            self._resource_name = resource_name
        else:
            self._resource_name = name
        self._configuration = configuration

    def parse(self, parsing_configuration={}):
        """Retrieve the properties of this component by parsing its corresponding source file.

        'parsing_configuration' is an optional dictionary that is merged with the component's base configuration and
        passed to the parsing function.

        This function returns a dictionary containing configuration information that can be used to render an RTOS
        template.

        """
        if isinstance(parsing_configuration, dict):
            configuration = self._configuration.copy()
            configuration.update(parsing_configuration)
        else:
            configuration = self._configuration

        component = None
        for name in [f.format(self._resource_name) for f in ['{0}.c', '{0}/{0}.c']]:
            try:
                component = Component.find(name)
                break
            except KeyError:
                pass
        if component is None:
            raise KeyError('Unable to find component "{}"'.format(self._resource_name))

        return parse_sectioned_file(component, configuration)


class ArchitectureComponent(Component):
    """This refinement of the Component class represents an architecture-specific component.

    This class encapsulates the act of finding the architecture-specific source file corresponding to this component.
    This is opposed to the base Component class which is unaware of architecture-specific file naming conventions.

    """
    def parse(self, arch):
        """Retrieve the properties of this component by parsing its architecture-specific source file.

        'arch', an instance of Architecture, identifies the architecture of the source file to parse.

        Otherwise, this function behaves as Component.parse().

        """
        assert isinstance(arch, Architecture)

        component = None
        for name in [f.format(arch.name, self._resource_name) for f in ['{0}-{1}/{0}-{1}.c', '{1}-{0}.c']]:
            try:
                component = Component.find(name)
                break
            except KeyError:
                pass
        if component is None:
            raise KeyError('Unable to find component "{}" for architecture {}'.format(self._resource_name, arch.name))

        return parse_sectioned_file(component, self._configuration)


#
# unittest related functionality. Note: This may be refactored in to a
# different module in the future.
#
# This reuses the bulk of the built-in unittest module, but adds some
# useful additional function inspired by the 'nose' testing framework.
#
# The major change is the manner in which test discovery occurs.
# This change was made to make the discovery more robust, simpler to
# understand, and avoid any sys.path magic.
#
# Also, unlike nose and the unittest modules, test discovery and test
# selection are handled in different phases. First, all tests for a
# given list of modules-under-test are found, then a sub-set of these
# tests may be chosen through a very simple test selection algorithm.
#
# Discovery is the first phase, which given a list of modules to test,
# finds all the testcases associated with the module. All the
# modules-under-test must be importable (i.e: they must exist within
# the current sys.path). The discovery process makes no changes to
# sys.path. Test discovery relies on tests existing in a fixed location
# for each list module. When a module is a package, all tests should
# live in a 'test' subpackage. (The subpackage must provide an __all__
# variable.) For standard modules the test is expected to reside within
# a '<modulename>_test'.
#
# Discovery expects tests to exist, and will raise ImportErrors if the
# expected test modules are not found.
#
# In a similar manner to nose, simple test functions and test generators
# are discovered as well as standard test classes that inherit from
# unittest.TestCase.
#
# Testcase generators should yield tuples of the form:
#
#   <gen-name>, func, args...
#
# The <gen-name> is appended to the name of the generator when determining
# the test case name.
#
# The second major change is the way in which testcases are named, and
# the way in which those names are reported when running the tests.
#
# All testcases have a unique name within the overall testrun. The exact
# way in which a testcase name is formed depends on the type of testcase.
# See the testcase_name function for details. Names should not contains
# spaces which should make it simple to provide test names on the command
# line. The improved testnames are printed during reporting, making it very
# simple to rerun a failing test on the command line.
#
# Finally, there is a specific test selection phase when running tests
# that is independent of the discovery phase. By default all testcases
# are selected, however the user may provide a specific list to run.
# Selection can be done via exact-match, prefix-match or regular expression
# matching.
#
def teamcityskip(test):
    """Use this decorator to skip a test when executing on TeamCity."""
    @functools.wraps(test)
    def wrapper():
        if IN_TEAMCITY:
            raise SkipTest
        return test()

    return wrapper


def ispackage(object):
    """Return true if the object is a package."""
    return inspect.ismodule(object) and hasattr(object, '__path__')


def testcase_matches(testcase, test_desc):
    """Return true if the testcase matches the test description.

    Matches if:
     - the test description is an exact match for the testcase name
     - the test description ends with '.' or ':' and is a prefix of the name
     - the test description is a regular expression and matches the name

    """
    name = testcase_name(testcase)

    r = False
    r = r or name == test_desc
    r = r or test_desc[-1] in '.:' and name.startswith(test_desc)
    r = r or re.match(test_desc + '$', name)
    return r


def testcase_name(testcase):
    """Return the name of a testcase.

    The format of the testcase name depends on the type of testcase.

    For class based testcases the format is <module_name>.<class_name>.<method_name>

    For function based testcases the format is <module_name>.<function_name>

    For generated testcases the format is <module_name>.<generator_name>:<description>

    """
    if isinstance(testcase, unittest.suite._ErrorHolder):
        return str(testcase)
    return testcase._testcase_name


class SimpleTestNameResult(unittest.TextTestResult):
    """SimpleTestNameResult extends the standard TextTestResult so that it prints
    the testcase's name rather than any docstring.

    See also: testcase_name

    """
    def getDescription(self, testcase):
        return testcase_name(testcase)


def testsuite_list(suite, file=sys.stdout):
    """Print all the contents of a testsuite to 'file' (defaults to sys.stdout)"""
    for test in suite:
        print(testcase_name(test), file=file)


class MethodTestCase(unittest.FunctionTestCase):
    def __init__(self, method, cls):
        super().__init__(method)
        self.cls = cls


class TestSuite(unittest.TestSuite):
    def __init__(self, tests):
        self._classes_setup = set()
        super().__init__(tests)

    def _handleClassSetUp(self, test, result):
        if isinstance(test, MethodTestCase):
            if test.cls not in self._classes_setup:
                test.cls.setUpClass()
                self._classes_setup.add(test.cls)
        return super()._handleClassSetUp(test, result)


def discover_tests_class(cls):
    """Generate testcases from the class 'cls'."""
    for name, method in [(name, getattr(cls, name)) for name in dir(cls)
                         if re.match('test.*', name)]:
        if callable(method):
            if issubclass(cls, unittest.TestCase):
                testcase = cls(name)
                testcase._testcase_name = "{}.{}.{}".format(cls.__module__, cls.__name__, name)
                yield testcase
            else:
                if inspect.isgeneratorfunction(method):
                    gen = getattr(cls(), name)
                    for name, *test in gen():
                        f = functools.partial(*test)
                        testcase = MethodTestCase(f, cls)
                        testcase._testcase_name = "{}.{}.{}".format(cls.__module__, cls.__name__, name)
                        yield testcase
                else:
                    testcase = MethodTestCase(getattr(cls(), name), cls)
                    testcase._testcase_name = "{}.{}.{}".format(cls.__module__, cls.__name__, name)
                    yield testcase


def discover_tests_module(module):
    """Generate testcases from the module."""
    for name, obj in [(name, getattr(module, name)) for name in dir(module)]:
        if inspect.isclass(obj) and (issubclass(obj, unittest.TestCase) or re.match('test.*', name)):
            yield from discover_tests_class(obj)
        elif callable(obj) and re.match('test_.*', name):
            if inspect.isgeneratorfunction(obj):
                for name, *test in obj():
                    f = functools.partial(*test)
                    testcase = unittest.FunctionTestCase(f)
                    testcase._testcase_name = "{}.{}:{}".format(obj.__module__, obj.__name__, name)
                    yield testcase
            else:
                testcase = unittest.FunctionTestCase(obj)
                testcase._testcase_name = "{}.{}".format(obj.__module__, obj.__name__)
                yield testcase


def discover_tests(*mut_names):
    """Discover all the associated tests for a given list of modules-under-test.

    This function will import all the listed modules (so these modules must be
    available within the path.

    Additionally, it will attempted to import all the tests for each module.

    If the module is a package, it is expected that all the tests will reside
    in modules within a 'test' subpackage.

    If this module isn't a package, it is expected that all the tests will reside
    in a module called 'test_<modulename>'.

    """
    muts = [(mut_name, importlib.import_module(mut_name)) for mut_name in mut_names]
    for mut_name, mut in muts:
        if ispackage(mut):
            test_package = importlib.import_module('{}.test'.format(mut.__name__))
            test_module_names = ['{}.{}'.format(test_package.__name__, m)
                                 for m in
                                 test_package.__all__] + [test_package.__name__]
            for tmn in test_module_names:
                yield from discover_tests_module(importlib.import_module(tmn))
        else:
            try:
                test_module = importlib.import_module('{}_test'.format(mut_name))
                yield from discover_tests_module(test_module)
            except ImportError:
                pass


def _prj_test():
    """Execute prj_test with a set of default arguments.

    This is used when calling prj_test internally (e.g: from build).

    """
    Args = collections.namedtuple('Args', ('verbose', 'quiet', 'tests', 'list'))
    args = Args(verbose=False, quiet=False, tests=[], list=False)
    return prj_test(args)


def prj_test(args):
    """Run tests associated with prj modules."""
    modules = ['prj', 'util']
    directories = [os.path.join('prj', 'app'),
                   os.path.join('prj', 'app', 'pystache'),
                   os.path.join('prj', 'app', 'lib')]

    return run_module_tests_with_args(modules, directories, args)


def x_test(args):
    """Run x-related tests."""
    modules = ['x']
    directories = ['.']

    return run_module_tests_with_args(modules, directories, args)


def rtos_test(args):
    """Run rtos unit tests."""
    modules = ['rtos']
    directories = ['.']

    return run_module_tests_with_args(modules, directories, args)


def run_module_tests_with_args(modules, directories, args):
    """Call a fixed set of modules in specific directories, deriving all input for a call to run_module_tests() from
    the given command line arguments.

    See `run_modules_tests` for more information.

    """
    patterns = args.tests
    verbosity = 0
    if args.verbose:
        verbosity = 1
    if args.quiet:
        verbosity = -1
    print_only = args.list

    return run_module_tests(modules, directories, patterns, verbosity, print_only)


def run_module_tests(modules, directories, patterns=[], verbosity=0, print_only=False):
    """Discover and run the tests associated with the given modules and located in the given directories.

    'modules' is list of module names as a sequence of strings.
    Only tests related to these modules are to be discovered.

    'directories' is a list of relative directory names as a sequence of strings.
    Only tests located in these directories are to be discovered.

    'patterns' is a list of test name patterns as a sequence of strings.
    If 'patterns' is not empty, only the tests whose names match one of the patterns are honored and all other
    discovered tests are ignored.
    If 'patterns' is empty, all discovered tests are honored.

    The integer 'verbosity' controls the amount of generated console output when executing the tests.
    A value of 0 selects the default verbosity level, positive values increase it, negative values reduce it.

    If the boolean 'print_only' is True, the discovered tests are printed on the console but not executed.

    Returns a process exit code suitable for passing to sys.exit().
    The exit code represents the number of tests that failed (0 indicating all tests passed).
    If more than 127 tests failed, then 127 will be returned.
    127 is the largest, portable value that can be returned via sys.exit().

    """
    result = 0

    paths = [top_path(dir) for dir in directories]
    if all(map(os.path.exists, paths)):
        with python_path(*paths):
            all_tests = discover_tests(*modules)

            if patterns:
                tests = (t for t in all_tests if any(testcase_matches(t, p) for p in patterns))
            else:
                tests = all_tests

            suite = TestSuite(tests)

            if print_only:
                testsuite_list(suite)
            else:
                BASE_VERBOSITY = 1
                runner = unittest.TextTestRunner(resultclass=SimpleTestNameResult,
                                                 verbosity=BASE_VERBOSITY + verbosity)
                run_result = runner.run(suite)
                result = min(len(run_result.failures), 127)

    return result


def prj_build(args):
    if sys.platform == 'darwin':
        host = 'x86_64-apple-darwin'
        extras = ['-framework', 'CoreFoundation', '-lz']
    elif sys.platform == 'linux':
        host = 'x86_64-unknown-linux-gnu'
        extras = ['-lz', '-lm', '-lpthread', '-lrt', '-ldl', '-lcrypt', '-lutil']
    elif sys.platform == 'win32':
        host = 'win32'
    else:
        print("Building prj currently unsupported on {}".format(sys.platform))
        return 1

    prj_build_path = top_path('prj_build_{}'.format(host))
    os.makedirs(prj_build_path, exist_ok=True)

    if sys.platform == 'win32':
        prj_build_win32(prj_build_path)
    else:
        with chdir(prj_build_path):
            ice.create_lib('prj', '../prj/app', main='prj')
            ice.create_lib('prjlib', '../prj/app/lib')
            ice.create_lib('pystache', '../prj/app/pystache',
                           excluded=['setup', 'pystache.tests', 'pystache.commands'])
            ice.create_lib('ply', '../prj/app/ply', excluded=['setup'])
            ice.create_stdlib()
            ice.create_app(['stdlib', 'prj', 'prjlib', 'pystache', 'ply'])

            cmd = ['gcc', '*.c', '-o', 'prj', '-I../tools/include/python3.3m/',
                   '-I../tools/{}/include/python3.3m/'.format(host),
                   '-L../tools/{}/lib/python3.3/config-3.3m'.format(host),
                   '-lpython3.3m']
            cmd += extras

            cmd = ' '.join(cmd)
            r = os.system(cmd)
            if r != 0:
                print("Error building {}. cmd={}. ".format(show_exit(r), cmd))


def prj_build_win32(output_dir):
    """Create a distributable version of prj.py.

    We currently do not have the infrastructure in place to statically compile and link prj.py and its dependencies
    against the complete python interpreter.

    However, it is still desirable to create only a single resource that can stand alone given an installed python
    interpreter.
    Therefore, collect prj and its dependencies in a zip file that is executable by the python interpreter.

    """
    with zipfile.ZipFile(os.path.join(output_dir, 'prj'), mode='w') as zip:
        top = os.path.abspath(base_path('prj', 'app'))
        top_len = len(top)
        for dir_path, dir_names, file_names in os.walk(top):
            archive_dir_path = os.path.relpath(dir_path, top)
            for file_name in file_names:
                file_path = os.path.join(dir_path, file_name)
                if dir_path == top and file_name == 'prj.py':
                    # The python interpreter expects to be informed about the main file in the zip file by naming it
                    # __main__.py
                    archive_file_path = os.path.join(archive_dir_path, '__main__.py')
                else:
                    archive_file_path = os.path.join(archive_dir_path, file_name)
                zip.write(file_path, archive_file_path)
    with open(os.path.join(output_dir, 'prj.bat'), 'w') as f:
        f.write('@ECHO OFF\npython %~dp0\\prj')


def build(args):
    # Generate RTOSes
    for rtos_name, arch_names in configurations.items():
        generate_rtos_module(skeletons[rtos_name], [architectures[arch] for arch in arch_names])


review_template = """Breakaway Task Review
=======================

Task name: %(branch)s
Version reviewed: %(sha)s
Reviewer: %(reviewer)s
Date: %(date)s
Conclusion: Accepted/Rework

Overall comments:


Specific comments
=================

Location: filename:linenum
Comment:

Location: filename:linenum
Comment:
"""


def new_review(args):
    """Create a new review for the current branch."""
    # Check the directory is clean
    status = subprocess.check_output(['git', 'status', '--porcelain'], cwd=topdir)
    if status != b'':
        print("Can't commit while directory is dirty. Aborting.")
        return 1

    branch = subprocess.check_output(['git', 'symbolic-ref', 'HEAD'], cwd=topdir).decode().strip().split('/')[-1]
    review_dir = top_path(os.path.join('pm', 'reviews', branch))

    sha = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=topdir).decode().strip()

    if os.path.exists(review_dir):
        review_number = sorted([int(re.match(r'review-([0-9]+)..*', f).group(1))
                                for f in os.listdir(review_dir)])[-1] + 1
    else:
        review_number = 0

    msg = "Creating review [%d] for branch [%s] with reviewers %s: (y/n) " % (review_number, branch, args.reviewers)
    x = input(msg)
    if x != 'y':
        print("aborted")
        return 1

    date = datetime.datetime.now().strftime('%Y-%m-%d')
    os.makedirs(review_dir, exist_ok=True)
    params = {
        'branch': branch,
        'sha': sha,
        'date': date
    }
    review_files = []
    for reviewer in args.reviewers:
        review_fn = os.path.join(review_dir, 'review-%d.%s' % (review_number, reviewer))
        review_files.append(review_fn)
        with open(review_fn, 'w', newline='\n') as f:
            params['reviewer'] = reviewer
            f.write(review_template % params)

    git = Git(local_repository=topdir)
    # now, git add, git commit -m <msg> and git push.
    msg = 'Review request {} for {}'.format(review_number, branch)
    git.add(review_files)
    git.commit(msg)
    git.push(branch, branch)


task_template = """Task: {}
==============================================================================

Goals
--------

"""


def new_task(args):
    """Create a new task."""
    remote = 'origin'
    branch_from = remote + '/development'
    tasks_dir = top_path('pm', 'tasks')

    git = Git(local_repository=topdir)
    if not git.working_dir_clean():
        print("Working directory must be clean before creating a new task.")
        return 1

    if args.fetch:
        # Ensure that we have the very last origin/development to branch
        # from.
        git.fetch()

    fullname = gen_tag() + '-' + args.taskname
    git.branch(fullname, branch_from, track=False)
    git.push(fullname, fullname, set_upstream=True)
    git.checkout(fullname)

    task_fn = os.path.join(tasks_dir, fullname)
    with open(task_fn, 'w', newline='\n') as f:
        f.write(task_template.format(fullname))

    print("Edit file: {} then add/commit/push.".format(task_fn))
    print('Suggest committing as: git commit -m "New task: {}"'.format(fullname))


def parse_sectioned_file(fn, config={}):
    """Given a sectioned C-like file, returns a dictionary of { section: content }

    For example an input of:
    /*| foo |*/
    foo data....

    /*| bar |*/
    bar data....

    Would produce:

    { 'foo' : "foo data....", 'bar' : "bar data...." }
    """

    with open(fn) as f:
        sections = {}
        current_lines = None
        for line in f.readlines():
            line = line.rstrip()

            if line.startswith('/*|') and line.endswith('|*/'):
                section = line[3:-3].strip()
                current_lines = []
                sections[section] = current_lines
            elif current_lines is not None:
                current_lines.append(line)

    for key, value in sections.items():
        sections[key] = render_data('\n'.join(value).rstrip(), "{}: Section {}".format(fn, key), config)

    for s in EXPECTED_SECTIONS:
        if s not in sections:
            raise Exception("Couldn't find exepcted section '{}' in file: '{}'".format(s, fn))

    return sections


def render_data(in_data, name, config):
    """Render input data (`in_data`) using a given `config`. The result is returned."""
    pystache.defaults.MISSING_TAGS = 'strict'
    pystache.defaults.DELIMITERS = ('[[', ']]')
    pystache.defaults.TAG_ESCAPE = lambda u: u
    return pystache.render(in_data, config, name=name)


class Architecture:
    """Represents the properties of a target architecture for which an RtosModule can be generated."""
    def __init__(self, name, configuration):
        assert isinstance(name, str)
        assert isinstance(configuration, dict)
        self.name = name
        self.configuration = configuration


class RtosSkeleton:
    """Represents an RTOS variant as defined by a set of components / functionalities.

    For example, the specific RTOS variant gatria consists exactly of a context-switch and a scheduler component.

    This class encapsulates the act of deriving an RtosModule for a specific configuration and target architecture.

    """
    def __init__(self, name, schema, components, configuration={}):
        """Create an RTOS skeleton based on its core properties.

        'name', a string, is the unique name of the RTOS skeleton.

        'components', a sequence of Component instances, is the set of components that define this RTOS variant.

        'configuration', a dictionary, contains configuration information specific to this RTOS variant.
        It is used when generating an RtosModule from this skeleton.

        """
        assert isinstance(name, str)
        assert isinstance(components, list)
        assert isinstance(configuration, dict)
        self.name = name
        self.schema = schema
        self._components = components
        self._configuration = configuration

    def get_module_sections(self, arch):
        """Retrieve the sections necessary to generate an RtosModule from this skeleton.

        """
        component_sections = [c.parse(arch) for c in self._components]
        return  {s: '\n'.join(c[s] for c in component_sections)
                 for s in EXPECTED_SECTIONS}

    def create_configured_module(self, arch):
        """Retrieve module configuration information and create a corresponding RtosModule instance.

        This is only a convenience function.

        """
        return RtosModule(self.name, self.schema, arch, self.get_module_sections(arch))


class RtosModule:
    """Represents an instance of an RtosSkeleton (or RTOS variant) with a specific configuration, in particular for a
    specific target architecture.

    This class encapsulates the act of rendering an RTOS template given an RTOS configuration into a module on disk.

    """
    def __init__(self, name, schema, arch, sections):
        """Create an RtosModule instance.

        'name', a string, is the name of the RTOS, i.e., the same as the underlying RtosSkeleton.

        'arch', an instance of Architecture, is the architecture this module is targetted for.

        'sections', a dictionary, containing all the merged sections from the RTOS components.
        It is typically obtained via RtosSkeleton.get_module_configuration().

        """
        assert isinstance(name, str)
        assert isinstance(arch, Architecture)
        assert isinstance(sections, dict)
        self._name = name
        self._arch = arch
        self._schema = schema
        self._sections = sections

    @property
    def _module_name(self):
        return 'rtos-' + self._name

    @property
    def _module_dir(self):
        module_dir = base_path('packages', self._arch.name, self._module_name)
        os.makedirs(module_dir, exist_ok=True)
        return module_dir

    def generate(self):
        """Generate the RTOS module to disk, so it is available as a compile and link unit to projects."""
        self._render()

    def _render(self):
        source_output = os.path.join(self._module_dir, 'entity.c')
        header_output = os.path.join(self._module_dir, self._module_name + '.h')
        source_sections = ['headers', 'object_like_macros',
                           'type_definitions', 'structure_definitions',
                           'extern_definitions', 'function_definitions',
                           'state', 'function_like_macros',
                           'functions', 'public_functions']
        header_sections = ['public_headers', 'public_type_definitions',
                           'public_object_like_macros', 'public_function_like_macros',
                           'public_extern_definitions', 'public_function_definitions']
        sections = self._sections
        with open(source_output, 'w') as f:
            f.write(self._schema)
            for ss in source_sections:
                if ss == 'type_definitions':
                    data = sort_typedefs(sections[ss])
                else:
                    data = sections[ss]
                f.write(data)
                f.write('\n')
        with open(header_output, 'w') as f:
            mod_name = self._module_name.upper().replace('-', '_')
            f.write("#ifndef {}_H\n".format(mod_name))
            f.write("#define {}_H\n".format(mod_name))
            for ss in header_sections:
                if ss == 'public_type_definitions':
                    data = sections[ss]
                else:
                    data = sections[ss]
                f.write(data)
                f.write('\n')
            f.write("\n#endif /* {}_H */".format(mod_name))


def generate_rtos_module(skeleton, architectures):
    """Generate RTOS modules for several architectures from a given skeleton."""
    for arch in architectures:
        rtos_module = skeleton.create_configured_module(arch)
        rtos_module.generate()


def tasks(args):
    git = Git(local_repository=topdir)
    task_dir = top_path('pm', 'tasks')
    skipped_branches = ['development', 'master']
    task_names = set.union({t for t in os.listdir(task_dir) if
                            os.path.isfile(os.path.join(task_dir, t))},
                           {t.split('/')[0] for t in git.branches + git.origin_branches if
                            t.count('/') == 0 and t not in skipped_branches})

    print("flags| last commit          | +- vs origin | +- vs devel  | name")
    for t in sorted(task_names):
        task = Task(t, topdir, git)
        print(task.report_line())
        for rel in task.related_branches():
            if rel in skipped_branches:
                continue
            branch = 'origin/' + t
            if branch not in git.get_remote_branches():
                branch = t
            base_branch = 'origin/' + rel
            if base_branch not in git.get_remote_branches():
                base_branch = rel
            print("{}{} ({})".format(' ' * 60, rel, git.ahead_behind_string(branch, base_branch).strip()))

    for check_branch in skipped_branches:
        if check_branch in git.branches:
            dev_ab = git.ahead_behind_string(check_branch, 'origin/' + check_branch).strip()
            if dev_ab != '':
                print("Warning: {} branch not in sync with origin: {}".format(check_branch, dev_ab))

    print('')
    print("D: described in pm/tasks  L: local branch  R: remote branch  A: archived branch")


@contextmanager
def tarfile_open(name, mode, **kwargs):
    assert mode.startswith('w')
    with tarfile.open(name, mode, **kwargs) as f:
        try:
            yield f
        except:
            os.unlink(name)
            raise


class FileWithLicense:
    """FileWithLicense provides a read-only file-like object that automatically includes license text when reading
    from the underlying file object.

    The FileWithLicense object takes ownership of the underlying file object.
    The original file object should not be used after passing it to the FileWithLicense object.

    """
    def __init__(self, f, lic, xml_mode):
        XML_PROLOG = b'<?xml version="1.0" encoding="UTF-8" ?>\n'
        self._f = f
        self._read_license = True

        if xml_mode:
            lic = XML_PROLOG + lic
            file_header = f.read(len(XML_PROLOG))
            if file_header != XML_PROLOG:
                raise Exception("XML File: '{}' does not contain expected prolog: {} expected {}".
                                format(f.name, file_header, XML_PROLOG))

        if len(lic) > 0:
            self._read_license = False
            self._license_io = io.BytesIO(lic)

    def read(self, size):
        assert size > 0
        data = b''
        if not self._read_license:
            data = self._license_io.read(size)
            if len(data) < size:
                self._read_license = True
                size -= len(data)

        if self._read_license:
            data += self._f.read(size)

        return data

    def close(self):
        self._f.close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()


class LicenseOpener:
    """The license opener provides a single 'open' method, that can be used instead of the built-in 'open' function.

    This open will return a file-like object that modifies the underlying file to include an appropriate license
    header.

    The 'license' is passed to the object during construction.

    """
    def __init__(self, license):
        self.license = license

    def _get_lic(self, filename):
        lic = ''
        ext = os.path.splitext(filename)[1]
        is_xml = False

        if ext in ['.c', '.h', '.ld', '.s']:
            lic = '/*' + self.license + '*/\n'
        elif ext in ['.py']:
            lic = '"""' + self.license + '"""\n'
        elif ext in ['.prx']:
            lic = '<!--' + self.license + '-->\n'
            is_xml = True

        lic = lic.encode('utf8')

        return lic, is_xml

    def open(self, filename, mode):
        assert mode == 'rb'

        f = open(filename, mode)
        lic, is_xml = self._get_lic(filename)
        return FileWithLicense(f, lic, is_xml)

    def tar_info_filter(self, tarinfo):
        if tarinfo.isreg():
            lic, _ = self._get_lic(tarinfo.name)
            tarinfo.size += len(lic)
        return tar_info_filter(tarinfo)


def tar_info_filter(tarinfo):
    tarinfo.uname = 'brtos'
    tarinfo.gname = 'brtos'
    tarinfo.mtime = BASE_TIME
    tarinfo.uid = 1000
    tarinfo.gid = 1000
    return tarinfo


def tar_add_data(tf, arcname, data, ti_filter=None):
    """Directly add data to a tarfile.

    tf is a tarfile.TarFile object.
    arcname is the name the data will have in the archive.
    data is the raw data (which should be of type 'bytes').
    fi_filter filters the created TarInfo object. (In a similar manner to the tarfile.TarFile.add() method.

    """
    ti = tarfile.TarInfo(arcname)
    ti.size = len(data)
    if ti_filter:
        ti = ti_filter(ti)
    tf.addfile(ti, io.BytesIO(data))


def tar_gz_with_license(output, tree, prefix, license):

    """Create a tar.gz file named `output` from a specified directory tree.

    Any appropriate files have the specified license attached.

    When creating the tar.gz a standard set of meta-data will be used to help ensure things are consistent.

    """
    lo = LicenseOpener(license)
    try:
        with tarfile.open(output, 'w:gz', format=tarfile.GNU_FORMAT) as tf:
            tarfile.bltn_open = lo.open
            with chdir(tree):
                for f in os.listdir('.'):
                    tf.add(f, arcname='{}/{}'.format(prefix, f), filter=lo.tar_info_filter)
    finally:
        tarfile.bltn_open = open


class Package:
    """Represents a customer visible package.

    This is currently used mainly for release management.

    """
    @staticmethod
    def create_from_disk():
        """Return a dictionary that contains a Package instance for each package on disk in a 'package' directory.

        The dictionary keys are the package names.
        The dictionary values are the package instances.

        """
        pkgs = {}
        for pkg_parent_dir in base_to_top_paths('packages'):
            pkg_names = os.listdir(pkg_parent_dir)
            for pkg_name in pkg_names:
                pkg_path = os.path.join(pkg_parent_dir, pkg_name)
                if pkg_name in pkgs:
                    logging.warn('Overriding package {} with package {}'.format(pkgs[pkg_name].path, pkg_path))
                pkgs[pkg_name] = Package(pkg_path)
        return pkgs

    def __init__(self, path):
        assert os.path.isdir(path)
        self.path = path
        self.name = os.path.basename(self.path)


class ReleasePackage:
    """Represents a Package instance that is refined for a specific release configuration.

    Configuring a Package instance for release results in additional properties of a package, relevant when creating
    release files.

    """
    def __init__(self, package, release_configuration):
        self._pkg = package
        self._rls_cfg = release_configuration

    def get_name(self):
        return self._pkg.name

    def get_path(self):
        return self._pkg.path

    def get_archive_name(self):
        return '{}-{}'.format(self._pkg.name, self._rls_cfg.name)

    def get_path_in_archive(self):
        return 'share/packages/{}'.format(self._pkg.name)

    def get_license(self):
        return self._rls_cfg.license


def mk_partial(pkg):
    fn = top_path('release', 'partials', '{}.tar.gz'.format(pkg.get_archive_name()))
    src_prefix = 'share/packages/{}'.format(pkg.get_name())
    tar_gz_with_license(fn, pkg.get_path(), src_prefix, pkg.get_license())


def build_partials(args):
    build([])
    os.makedirs(top_path('release', 'partials'),  exist_ok=True)
    packages = Package.create_from_disk().values()
    for pkg in packages:
        for config in get_release_configs():
            release_package = ReleasePackage(pkg, config)
            mk_partial(release_package)


def build_manual(pkg):
    manual_file = os.path.join(BASE_DIR, 'packages', pkg, '{}-manual'.format(pkg))
    if not os.path.exists(manual_file):
        print("Manual '{}' does not exist.".format(manual_file))
    else:
        print("Transforming manual '{}'".format(manual_file))


def build_manuals(args):
    build([])
    packages = os.listdir(os.path.join(BASE_DIR, 'packages'))
    for pkg in packages:
        build_manual(pkg)


class ReleaseMeta(type):
    """A pretty-printing meta-class for the Release class."""
    def __str__(cls):
        return '{}-{}'.format(cls.name, cls.version)


class Release(metaclass=ReleaseMeta):
    """The Release base class is used by the release configuration."""
    packages = []
    platforms = []
    version = None
    name = None
    enabled = False
    license = None
    top_level_license = None
    extra_files = []


def get_release_configs():
    """Return a list of release configs."""
    import release_cfg
    maybe_configs = [getattr(release_cfg, cfg) for cfg in dir(release_cfg)]
    configs = [cfg for cfg in maybe_configs if inspect.isclass(cfg) and issubclass(cfg, Release)]
    enabled_configs = [cfg for cfg in configs if cfg.enabled]
    return enabled_configs


def build_single_release(config):
    """Build a release archive for a specific release configuration."""
    basename = 'brtos-{}-{}'.format(config.name, config.version)
    logging.info("Building {}".format(basename))
    with tarfile_open(top_path('release', '{}.tar.gz'.format(basename)), 'w:gz', format=tarfile.GNU_FORMAT) as tf:
        for pkg in config.packages:
            release_file_path = top_path('release', 'partials', '{}-{}.tar.gz'.format(pkg, config.name))
            with tarfile.open(release_file_path, 'r:gz') as in_f:
                for m in in_f.getmembers():
                    m_f = in_f.extractfile(m)
                    m.name = basename + '/' + m.name
                    tf.addfile(m, m_f)
        for plat in config.platforms:
            arcname = '{}/{}/bin/prj'.format(basename, plat)
            tf.add('prj_build_{}/prj'.format(plat), arcname=arcname, filter=tar_info_filter)
        if config.top_level_license is not None:
            tar_add_data(tf, '{}/LICENSE'.format(basename),
                         config.top_level_license.encode('utf8'),
                         tar_info_filter)

        for arcname, filename in config.extra_files:
            tf.add(filename, arcname='{}/{}'.format(basename, arcname), filter=tar_info_filter)

        if 'TEAMCITY_VERSION' in os.environ:
            build_info = os.environ['BUILD_VCS_NUMBER']
        else:
            g = Git()
            build_info = g.branch_hash()
            if not g.working_dir_clean():
                build_info += "-unclean"
        build_info += '\n'
        tar_add_data(tf, '{}/build_info'.format(basename), build_info.encode('utf8'), tar_info_filter)


def build_release(args):
    """Implement the build-release command.

    Build release takes the various partial releases, and combines them in to a single tar file.

    Additionally, it takes the binary 'prj' files and adds it to the appropriate place in the release tar file.

    """
    for config in get_release_configs():
        try:
            build_single_release(config)
        except FileNotFoundError as e:
            logging.warning("Unable to build '{}'. File not found: '{}'".format(config, e.filename))


def release_test_one(archive):
    """Test a single archive

    This is primarily a sanity check of the actual release file. Release
    files are only made after other testing has been successfully performed.

    Currently it simply does some sanitfy checking on the tar file to ensure it is consistent.

    In the future additional testing will be performed.

    """
    project_prj_template = """<?xml version="1.0" encoding="UTF-8" ?>
<project>
<search-path>{}</search-path>
</project>
"""

    rel_file = os.path.abspath(archive)
    with tarfile.open(rel_file, 'r:gz') as tf:
        for m in tf.getmembers():
            if m.gid != 1000:
                raise Exception("m.gid != 1000 {} -- {}".format(m.gid, m.name))
            if m.uid != 1000:
                raise Exception("m.uid != 1000 {} -- {}".format(m.uid, m.name))
            if m.mtime != BASE_TIME:
                raise Exception("m.gid != BASE_TIME({}) {} -- {}".format(m.mtime, BASE_TIME, m.name))

    # In the future this hard-coding will be removed.
    platform = 'x86_64-unknown-linux-gnu'  # 'x86_64-apple-darwin'

    with tempdir() as td:
        with chdir(td):
            subprocess.check_call("tar xf {}".format(rel_file).split())
            release_dir = os.path.splitext(os.path.splitext(os.path.basename(archive))[0])[0]
            if not os.path.isdir(release_dir):
                raise RuntimeError("Release archive does not extract into top directory with the same name as the "
                                   "base name of the archive ({})".format(release_dir))
            with chdir(release_dir):
                os.system("./{}/bin/prj".format(platform))
                pkgs = []
                pkg_root = './share/packages/'
                for root, _dir, files in os.walk(pkg_root):
                    for f in files:
                        if f.endswith('.prx'):
                            pkg = os.path.join(root, f)[len(pkg_root):-4].replace(os.sep, '.')
                            print(pkg)
                            pkgs.append(pkg)
                with open('project.prj', 'w') as f:
                    f.write(project_prj_template.format(pkg_root))
                for pkg in pkgs:
                    x = os.system("./{}/bin/prj build {}".format(platform, pkg))
                    if x != 0:
                        raise Exception("Build failed.")


def release_test(args):
    """Implement the test-release command.

    This command is used to perform sanity checks and testing of the full release.

    """
    for rel in glob(top_path('release', '*.tar.gz')):
        release_test_one(rel)


class Git:
    """
    Represents common state applicable to a series of git invocations and provides a pythonic interface to git.
    """
    def __init__(self, local_repository=os.getcwd(), remote_repository='origin'):
        """
        Create a Git instance with which all commands operate with the given local and remote repositories.
        """
        assert isinstance(local_repository, str)
        assert isinstance(remote_repository, str)
        # Check whether the local repository is a directory managed by git.
        # Note that '.git' can be a directory in case of a git repository or a file in case of a git submodule
        assert os.path.exists(os.path.join(local_repository, '.git'))
        self.local_repository = local_repository
        self.remote_repository = remote_repository
        self._sep = None
        self._branches = None
        self._remote_branches = None

    def convert_path_separators(self, files):
        """
        If necessary, convert the path separators in the path or paths given in 'files' to the path separator expected
        by the command-line git tool.
        These separators differ when a cygwin git command-line tool is used with a native Windows python installation.
        """
        assert isinstance(files, (str, list))
        if isinstance(files, str):
            return files.replace(os.sep, self.sep)
        else:
            return [file.replace(os.sep, self.sep) for file in files]

    @property
    def sep(self):
        """
        Return the (potentially cached) path separator expected by the git command-line tool.
        """
        if self._sep is None:
            self._sep = self._get_sep()
        return self._sep

    def _get_sep(self):
        """
        Determine the path separator expected by the git command-line tool.
        """
        output = self._do(['ls-tree', '-r', '--name-only', 'HEAD:pm/tasks'])
        for line in output.splitlines():
            if line.startswith('completed'):
                line = line.replace('completed', '', 1)
                return line[0]
        raise LookupError('git ls-tree does not list any files in pm/tasks/completed as expected')

    @property
    def branches(self):
        """List of local branches."""
        if self._branches is None:
            self._branches = self._get_branches()
        return self._branches

    def _get_branches(self):
        """Return a list of local branches."""
        return [x[2:] for x in self._do(['branch'], as_lines=True)]

    @property
    def origin_branches(self):
        """List of origin-remote branches"""
        return self.get_remote_branches()

    def get_remote_branches(self, remote='origin'):
        """Return a list of remote branches. Remote defaults to 'origin'."""
        if self._remote_branches is None:
            self._remote_branches = [x[2:].strip() for x in self._do(['branch', '-r'], as_lines=True)]
        return [x[len(remote) + 1:] for x in self._remote_branches if x.startswith(remote + '/')]

    def _do(self, parameters, as_lines=False):
        """
        Execute the git command line tool with the given command-line parameters and return the console output as a
        string.
        """
        assert type(parameters) == list
        raw_data = subprocess.check_output(['git'] + parameters, cwd=self.local_repository).decode()
        if as_lines:
            return raw_data.splitlines()
        else:
            return raw_data

    def get_active_branch(self):
        """
        Determine the currently active branch in the local git repository and return its name as a string.
        """
        pattern = '* '
        for line in self._do(['branch'], as_lines=True):
            if line.startswith(pattern):
                return line.split(' ', maxsplit=1)[1].strip()
        raise LookupError('No active branch in git repository ' + self.local_repository)

    def branch(self, name, start_point=None, *, track=None):
        """Create a new branch, optionally from a specific start point.

        If track is set to True, then '--track' will be passed to git.
        If track is set to False, then '--no-track' will be passed to git.
        If track is None, then no tracking flag will be passed to git.

        """
        params = ['branch']
        if not track is None:
            params.append('--track' if track else '--no-track')
        params.append(name)
        if not start_point is None:
            params.append(start_point)
        return self._do(params)

    def set_upstream(self, upstream, branch=None):
        """Set the upstream / tracking branch of a given branch.

        If branch is None, it defaults to the current branch.

        """
        params = ['branch', '-u', upstream]
        if branch:
            params.append(branch)

        return self._do(params)

    def checkout(self, revid):
        """
        Check out the specified revision ID (typically a branch name) in the local repository.
        """
        assert isinstance(revid, str)
        return self._do(['checkout', revid])

    def merge_into_active_branch(self, revid):
        """
        Merge the specified revision ID into the currently active branch.
        """
        assert isinstance(revid, str)
        return self._do(['merge', revid])

    def fetch(self):
        """Fetch from the remote origin."""
        return self._do(['fetch'])

    def push(self, src=None, dst=None, *, force=False, set_upstream=False):
        """Push the local revision 'src' into the remote branch 'dst', optionally forcing the update.

        If 'set_upstream' evaluates to True, 'dst' is set as the upstream / tracking branch of 'src'.

        """
        assert src is None or isinstance(src, str)
        assert dst is None or isinstance(dst, str)
        assert isinstance(force, bool)
        revspec = ''
        if src:
            revspec = src
        if dst:
            revspec += ':' + dst
        if revspec == '':
            revspec_args = []
        else:
            revspec_args = [revspec]
        if force:
            force_option = ['--force']
        else:
            force_option = []
        if set_upstream:
            set_upstream_option = ['-u']
        else:
            set_upstream_option = []
        return self._do(['push'] + force_option + set_upstream_option + [self.remote_repository] + revspec_args)

    def move(self, src, dst):
        """
        Rename a local resource from its old name 'src' to its new name 'dst' or move a list of local files 'src' into
        a directory 'dst'.
        """
        assert isinstance(src, (str, list))
        assert isinstance(dst, str)
        if type(src) == str:
            src_list = [src]
        else:
            src_list = src
        return self._do(['mv'] + self.convert_path_separators(src_list) + [self.convert_path_separators(dst)])

    def add(self, files):
        """Add the list of files to the index in preparation of a future commit."""
        return self._do(['add'] + self.convert_path_separators(files))

    def commit(self, msg, files=None):
        """Commit the changes in the specified 'files' with the given 'message' to the currently active branch.

        If 'files' is None (or unspecified), all staged files are committed.

        """
        assert isinstance(msg, str)
        assert files is None or isinstance(files, list)
        if files is None:
            file_args = []
        else:
            file_args = self.convert_path_separators(files)
        return self._do(['commit', '-m', msg] + file_args)

    def rename_branch(self, src, dst):
        """
        Rename a local branch from its current name 'src' to the new name 'dst'.
        """
        assert isinstance(src, str)
        assert isinstance(dst, str)
        return self._do(['branch', '-m', src, dst])

    def delete_remote_branch(self, branch):
        assert isinstance(branch, str)
        return self.push(dst=branch)

    def ahead_list(self, branch, base_branch):
        """Return a list of SHAs for the commits that are in branch, but not base_branch"""
        return self._do(['log', '{}..{}'.format(base_branch, branch), '--pretty=format:%H'], as_lines=True)

    def branch_contains(self, commits):
        """Return a set of branches that contain any of the commits."""
        contains = set()
        for c in commits:
            for b in self._do(['branch', '--contains', c], as_lines=True):
                contains.add(b[2:])
        return contains

    def count_commits(self, since, until):
        """Return the number of commit between two commits 'since' and 'until'.

        See git log --help for more details.

        """
        return len(self._do(['log', '{}..{}'.format(since, until), '--pretty=oneline'], as_lines=True))

    def ahead_behind(self, branch, base_branch):
        """Return the a tuple for how many commits ahead/behind a branch is when compared
        to a base_branch.

        """
        return self.count_commits(base_branch, branch), self.count_commits(branch, base_branch)

    def ahead_behind_string(self, branch, base_branch):
        """Format git_ahead_behind() as a string for presentation to the user.

        """
        ahead, behind = self.ahead_behind(branch, base_branch)
        r = ''
        if behind > 0:
            r = '-{:<4} '.format(behind)
        else:
            r = '      '
        if ahead > 0:
            r += '+{:<4}'.format(ahead)
        return r

    def _log_pretty(self, pretty_fmt, branch=None):
        """Return information from the latest commit with a specified `pretty` format.

        The log from a specified branch may be specified.
        See `git log` man page for possible pretty formats.

        """
        # Future directions: Rather than just the latest commit, allow the caller
        # specify the number of commits. This requires additional parsing of the
        # result to return a list, rather than just a single item.
        # Additionally, the caller could pass a 'conversion' function which would
        # convert the string into a a more useful data-type.
        # As this method may be changed in the future, it is marked as a private
        # function (for now).
        cmd = ['log']
        if branch is not None:
            cmd.append(branch)
        cmd.append('-1')
        cmd.append('--pretty=format:{}'.format(pretty_fmt))
        return self._do(cmd).strip()

    def branch_date(self, branch=None):
        """Return the date of the latest commit on a given branch as a UNIX timestamp.

        The branch may be ommitted, in which case it defaults to the current head.

        """
        return int(self._log_pretty('%at', branch=branch))

    def branch_hash(self, branch=None):
        """Return the hash of the latest commit on a given branch as a UNIX timestamp.

        The branch may be ommitted, in which case it defaults to the current head.

        """
        return self._log_pretty('%H', branch=branch)

    def working_dir_clean(self):
        """Return True is the working directory is clean."""
        return self._do(['status', '--porcelain']) == ''


class Review:
    """
    Represents a review on a development task/branch.
    """
    def __init__(self, file_path):
        """
        Create a Review instance representing the review in the given 'file_path'.
        This provides easy access to the review's review round, author, and conclusion.
        """
        assert isinstance(file_path, str)
        assert os.path.isfile(file_path)
        self.file_path = file_path
        trunk, self.author = os.path.splitext(file_path)
        self.author = self.author[1:]
        relative_trunk = os.path.basename(trunk)
        self.round = int(relative_trunk.split('-')[-1])
        self._conclusion = None

    def _get_conclusion(self):
        """
        Determine the conclusion of this review.
        The conclusion can be expected to be one of 'Accepted/Rework' (i.e., the review has not been completed),
        'Accepted', or 'Rework'.
        """
        f = open(self.file_path)
        for line in f:
            if line.startswith('Conclusion: '):
                conclusion = line.split(':')[1].strip()
                f.close()
                return conclusion.lower()
        f.close()
        assert False

    @property
    def conclusion(self):
        """
        Return the conclusion of this review.
        The conclusion can be expected to be one of 'Accepted/Rework' (i.e., the review has not been completed),
        'Accepted', or 'Rework'.
        """
        if self._conclusion is None:
            self._conclusion = self._get_conclusion()
        return self._conclusion

    def is_accepted(self):
        return self.conclusion in ['accept', 'accepted']

    def is_rework(self):
        return self.conclusion == 'rework'

    def is_done(self):
        return self.conclusion != 'accepted/rework'


class InvalidTaskStateError(RuntimeError):
    """
    To be raised when a task state transition cannot be performed because the task is not in the appropriate source
    state.
    """
    pass


class Task:
    """
    Represents a development task.
    Each task is associated with several task-related resources, such as the corresponding git branch, a description
    file, and reviews.
    Over its life time, a task traverses several states (such as ready for implementation, complete and ready for
    integration, and integrated).
    This class implements some aspects of task management and helps automating state transitions.
    """
    @staticmethod
    def create(name=None, top_directory=None):
        """
        Create and return a new Task instance, falling back to defaults if the optional task name and repository
        directory are not specified.
        If 'top_directory' is not specified, it defaults to the current working directory which must be a valid local
        git repository.
        If 'name' is not specified, it defaults to the name of the active branch in the task's top directory.
        """
        if top_directory is None:
            top_directory = os.getcwd()
        # Note that '.git' can be a directory in case of a git repository or a file in case of a git submodule
        assert os.path.exists(os.path.join(os.getcwd(), '.git'))
        if name is None:
            # derive name from current git branch
            name = Git(top_directory).get_active_branch()
        assert name

        git = Git(local_repository=top_directory)
        task = Task(name, top_directory, git)
        assert os.path.exists(task.get_description_file_name())

        return task

    def __init__(self, name, top_directory, git):
        """
        Create a task with the given 'name' in the repository rooted in 'top_directory'.
        It is expected that the repository contains a git branch with same name as the task name and that there is a
        task description file in the pm/tasks directory, again, with the task name as file name.
        """
        assert isinstance(name, str)
        assert isinstance(top_directory, str)
        assert os.path.isdir(top_directory)
        self.name = name
        self.top_directory = top_directory
        self._reviews = None
        self._git = git
        self.is_local = name in git.branches
        self.is_remote = name in git.origin_branches
        self.is_archived_remote = 'archive/' + name in git.origin_branches
        self.is_pm = os.path.exists(os.path.join(top_directory, 'pm', 'tasks', name))

    def integrate(self, target_branch='development', archive_prefix='archive'):
        """
        Integrate this branch into the upstream branch 'target_branch' and archive it.
        A branch can only be successfully integrated after it has been reviewed and all reviewers have arrived at the
        'accepted' conclusion.
        """
        assert isinstance(target_branch, str)
        assert isinstance(archive_prefix, str)
        self._pre_integration_check()
        self._integrate(target_branch)
        self._archive(archive_prefix)

    def _pre_integration_check(self):
        """
        Check whether the current task is ready for integration.
        """
        try:
            self._check_is_active_branch()
            self._check_is_accepted()
        except InvalidTaskStateError as e:
            raise InvalidTaskStateError('Task {} is not ready for integration: {}'.format(self.name, e))

    def _check_is_active_branch(self):
        """
        Check whether this task is checked out as the active git branch in the task's repository.
        """
        active_branch = self._git.get_active_branch()
        if active_branch != self.name:
            raise InvalidTaskStateError('Task {} is not the active checked-out branch ({}) in repository {}'.
                                        format(self.name, active_branch, self.top_directory))

    def _check_is_accepted(self):
        """
        Check whether all authors of completed reviews arrive at the 'accepted' conclusion in their final reviews.
        """
        all_reviews = self._get_most_recent_reviews_from_all_authors()
        done_reviews = filter(Review.is_done, all_reviews)
        if len(list(done_reviews)) == 0:
            raise InvalidTaskStateError('Task {} has not been reviewed'.format(self.name))
        for review in done_reviews:
            if not review.is_accepted():
                raise InvalidTaskStateError('The conclusion of review {} for task {} is not "accepted"'.
                                            format(review.file_path, self.name))

    def _get_most_recent_reviews_from_all_authors(self):
        """
        For any reviewer having reviewed this task, determine the most recent review and return all of them as a list
        of Review instances.
        """
        reviews_by_author = {}
        for review in self.reviews:
            if review.author in reviews_by_author:
                if reviews_by_author[review.author].round < review.round:
                    reviews_by_author[review.author] = review
            else:
                reviews_by_author[review.author] = review
        return reviews_by_author.values()

    @property
    def reviews(self):
        """
        Return a list of Review instances that represents all reviews of this task, even uncompleted ones.
        """
        if not self._reviews:
            self._reviews = self._get_reviews()
        return self._reviews

    def _get_reviews(self):
        """
        Retrieve and return a list of Review instances that represents all reviews of this task, even uncompleted
        ones.
        """
        reviews = []
        review_directory = os.path.join(self.top_directory, 'pm', 'reviews', self.name)
        directory_contents = os.listdir(review_directory)
        directory_contents.sort()
        for relative_path in directory_contents:
            absolute_path = os.path.join(review_directory, relative_path)
            if os.path.isfile(absolute_path):
                review = Review(absolute_path)
                reviews.append(review)
        return reviews

    def _integrate(self, target_branch):
        """
        Integrate this task by merging it into the given git 'target_branch' and marking it as complete.
        The resulting new state of 'target_branch' is pushed to the remote repository.
        """
        assert isinstance(target_branch, str)
        self._git.checkout(target_branch)
        self._git.merge_into_active_branch(self.name)
        self._complete()
        self._git.push(target_branch, target_branch)

    def _complete(self):
        """
        Mark this taks as complete in the currently active git branch by moving the task description file into the
        'completed' sub-directory and committing the result.
        """
        src = os.path.join('pm', 'tasks', self.name)
        dst = os.path.join('pm', 'tasks', 'completed', self.name)
        self._git.move(src, dst)
        self._git.commit(msg='Mark task {} as completed'.format(self.name), files=[os.path.join('pm', 'tasks')])

    def _archive(self, archive_prefix):
        """
        Archive this task by renaming it with the given 'archive_prefix' in both the local and the remote
        repositories.
        """
        assert isinstance(archive_prefix, str)
        archived_name = archive_prefix + '/' + self.name
        self._git.rename_branch(self.name, archived_name)
        self._git.push(archived_name, archived_name)
        self._git.delete_remote_branch(self.name)

    @property
    def description(self):
        """
        Return the (potentially cached) description of this task as a string.
        """
        if self._description is None:
            self._description = self._get_description()
        return self._description

    def _get_description(self):
        """
        Retrieve the description of this task from the file system and return it as a string.
        """
        return open(self.get_description_file_name()).read()

    def get_description_file_name(self):
        """
        Return the name of the description file of this task as a string.
        """
        return os.path.join(self.top_directory, 'pm', 'tasks', self.name)

    def related_branches(self):
        """Return a set of branch names that are related to this branch.

        Another branch is related if it contains any of the same
        commits as this task's branch that are not already on the
        development branch.

        """
        if self.is_local:
            ahead_list = self._git.ahead_list(self.name, 'origin/development')
            if len(ahead_list) > 100:  # Some branches are so far diverged we ignore them
                return set()
            return self._git.branch_contains(ahead_list) - {self.name}
        return set()

    def report_line(self):
        """Return a one line summary of the task for report printing purposes."""
        date = self.last_commit()
        if date is None:
            date_str = ''
        else:
            date_str = time.strftime('%Y-%m-%d %H:%M', time.localtime(date))

        if self.is_local and self.is_remote:
            vs_remote = self._git.ahead_behind_string(self.name, 'origin/' + self.name)
        else:
            vs_remote = ''

        if self.is_remote:
            vs_dev = self._git.ahead_behind_string('origin/' + self.name, 'origin/development')
        else:
            vs_dev = ''

        return '{}{}{}{} | {:20} | {:12} | {:12} | {}'.format(
            'D' if self.is_pm else ' ',
            'L' if self.is_local else ' ',
            'R' if self.is_remote else ' ',
            'A' if self.is_archived_remote else ' ',
            date_str,
            vs_remote,
            vs_dev,
            self.name)

    def last_commit(self):
        """Return the date (as a UNIX timestamp) of the last commit.

        The last commit on the local branch is preferred over the remote branch.
        If the task has no branches, then None is returned.

        """
        if self.is_local:
            return self._git.branch_date(self.name)
        elif self.is_remote:
            return self._git.branch_date('origin/' + self.name)
        else:
            return None


def integrate(command_line_options):
    """
    Integrate a completed development task/branch into the main upstream branch.
    """
    task = Task.create(command_line_options.name, command_line_options.repo)
    task.integrate(command_line_options.target, command_line_options.archive)


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
}


sched_rr_test_schema = """/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="num_tasks" type="int"/>
  </schema>
</module>*/
"""

simple_mutex_test_schema = """/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-simple-mutex-test.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="num_mutexes" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="mutexes" type="list">
     <entry name="mutex" type="dict">
      <entry name="idx" type="int" />
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/
"""

acamar_schema = """/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-acamar.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="idx" type="int" />
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
  </schema>
</module>*/
"""

gatria_schema = """/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-gatria.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="num_mutexes" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="idx" type="int" />
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="mutexes" type="list">
     <entry name="mutex" type="dict">
      <entry name="idx" type="int" />
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/
"""

kraz_schema = """/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-kraz.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="signalset_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="num_mutexes" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="idx" type="int" />
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="mutexes" type="list">
     <entry name="mutex" type="dict">
      <entry name="idx" type="int" />
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/
"""

acrux_schema = """/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-acrux.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="irqeventid_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="num_mutexes" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="idx" type="int" />
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="irq_events" type="list">
     <entry name="irq_event" type="dict">
      <entry name="idx" type="int" />
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
   <entry name="mutexes" type="list">
     <entry name="mutex" type="dict">
      <entry name="idx" type="int" />
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/
"""

rigel_schema = """/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-rigel.h" code_gen="template" />
  </headers>
</module>*/
"""


CORE_SKELETONS = {
    'sched-rr-test': RtosSkeleton(
        'sched-rr-test', sched_rr_test_schema,
        [Component('sched-rr-test'),
         Component('sched', 'sched-rr', {'assume_runnable': False})]),
    'simple-mutex-test': RtosSkeleton(
        'simple-mutex-test', simple_mutex_test_schema,
        [Component('simple-mutex-test'),
         Component('mutex', 'simple-mutex')]),
    'acamar': RtosSkeleton(
        'acamar', acamar_schema,
        [Component('acamar'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch')]),
    'gatria': RtosSkeleton(
        'gatria', gatria_schema,
        [Component('gatria'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('mutex', 'simple-mutex')]),
    'kraz': RtosSkeleton(
        'kraz', kraz_schema,
        [Component('kraz'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('signal'),
         Component('mutex', 'simple-mutex')]),
    'acrux': RtosSkeleton(
        'acrux', acrux_schema,
        [Component('acrux'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         ArchitectureComponent('irq_event_arch', 'irq-event'),
         Component('irq_event', 'irq-event'),
         Component('mutex', 'simple-mutex')]),
    'rigel': RtosSkeleton(
        'rigel', rigel_schema,
        [Component('rigel'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('signal'),
         ArchitectureComponent('irq_event_arch', 'irq-event'),
         Component('irq_event', 'irq-event'),
         Component('mutex', 'simple-mutex')],
    ),
}


CORE_CONFIGURATIONS = {
    'sched-rr-test': ['posix'],
    'simple-mutex-test': ['posix'],
    'acamar': ['posix', 'armv7m'],
    'gatria': ['posix', 'armv7m'],
    'kraz': ['posix', 'armv7m'],
    'acrux': ['armv7m'],
    'rigel': ['armv7m'],
}


# client repositories may extend or override the following variables to control which configurations are available
architectures = CORE_ARCHITECTURES.copy()
skeletons = CORE_SKELETONS.copy()
configurations = CORE_CONFIGURATIONS.copy()


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    SUBCOMMAND_TABLE = {
        'check-pep8': check_pep8,
        'prj-test': prj_test,
        'prj-build': prj_build,
        'build': build,
        'test-release': release_test,
        'build-release': build_release,
        'build-partials': build_partials,
        'build-manuals': build_manuals,
        'new-review': new_review,
        'new-task': new_task,
        'tasks': tasks,
        'integrate': integrate,
        'x-test': x_test,
        'rtos-test': rtos_test,
    }

    # create the top-level parser
    parser = argparse.ArgumentParser(prog='p')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    # create the parser for the "prj.pep8" command
    subparsers.add_parser('tasks', help="List tasks")
    subparsers.add_parser('check-pep8', help='Run PEP8 on project Python files')
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

    subparsers.add_parser('build-release', help='Build final release')
    subparsers.add_parser('test-release', help='Test final release')
    subparsers.add_parser('build-partials', help='Build partial release files')
    subparsers.add_parser('build-manuals', help='Build PDF manuals')
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

    args = parser.parse_args()

    # Default to building
    if args.command is None:
        args.command = 'build'

    return SUBCOMMAND_TABLE[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
