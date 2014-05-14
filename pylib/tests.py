import os
import sys
import unittest
import subprocess
from contextlib import contextmanager

from .xunittest import discover_tests, TestSuite, SimpleTestNameResult, testcase_matches, testsuite_list


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


def pystache_test(args):
    """Run tests assocaited with pystache modules."""
    return subprocess.call([sys.executable, os.path.join('prj', 'app', 'pystache', 'test_pystache.py')])


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
    topdir = args.topdir

    return run_module_tests(modules, directories, patterns, verbosity, print_only, topdir)


def run_module_tests(modules, directories, patterns=[], verbosity=0, print_only=False, topdir=""):
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

    paths = [os.path.join(topdir, dir) for dir in directories]
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
