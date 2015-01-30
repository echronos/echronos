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

import os
import sys
import pep8
import logging
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

    return _run_module_tests_with_args(modules, directories, args)


def x_test(args):
    """Run x-related tests."""
    modules = ['x']
    directories = ['.']

    return _run_module_tests_with_args(modules, directories, args)


def pystache_test(_):
    """Run tests assocaited with pystache modules."""
    return subprocess.call([sys.executable, os.path.join('prj', 'app', 'pystache', 'test_pystache.py')])


def rtos_test(args):
    """Run rtos unit tests."""
    modules = ['rtos']
    directories = ['.']

    return _run_module_tests_with_args(modules, directories, args)


def _run_module_tests_with_args(modules, directories, args):
    """Call a fixed set of modules in specific directories, deriving all input for a call to _run_module_tests() from
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

    return _run_module_tests(modules, directories, patterns, verbosity, print_only, topdir)


def _run_module_tests(modules, directories, patterns=None, verbosity=0, print_only=False, topdir=""):
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
    if all([os.path.exists(p) for p in paths]):
        with _python_path(*paths):
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
def _python_path(*paths):
    """A context manager that adds (and removes) one or more directories from the Python path.

    This allows extending the Python path temporarily to load certain modules.
    The directories are expected as individual arguments, e.g., "with _python_path('foo', 'bar'):"

    """
    paths = [os.path.abspath(path) for path in paths]
    sys.path = paths + sys.path
    try:
        yield
    finally:
        del sys.path[:len(paths)]


class _TeamcityReport(pep8.StandardReport):
    """Collect results and print teamcity messages."""

    def __init__(self, options):
        super(_TeamcityReport, self).__init__(options)

    def get_file_results(self):
        ret = super(_TeamcityReport, self).get_file_results()
        if self.file_errors:
            self._teamcity("testFailed name='%s'" % self._test_name())
        self._teamcity("testFinished name='%s'" % self._test_name())
        return ret

    def init_file(self, filename, lines, expected, line_offset):
        ret = super(_TeamcityReport, self).init_file(filename, lines, expected, line_offset)
        self._teamcity("testStarted name='%s' captureStandardOutput='true'" % self._test_name())
        return ret

    @staticmethod
    def _teamcity(msg):
        print("##teamcity[{}]".format(msg))

    def _test_name(self):
        return self.filename[:-3].replace("|", "||").replace("'", "|'").replace("[", "|[") \
            .replace("]", "|]").replace("\n", "|n").replace("\r", "|r")


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
    excludes = ['external_tools', 'pystache', 'tools', 'ply'] + args.excludes
    exclude_patterns = ','.join(excludes)
    options = ['--exclude=' + exclude_patterns, '--max-line-length', '118', os.path.join(args.topdir, ".")]

    logging.info('pep8 check: ' + ' '.join(options))

    pep8style = pep8.StyleGuide(arglist=options)
    if args.teamcity:
        pep8style.init_report(_TeamcityReport)
    report = pep8style.check_files()
    if report.total_errors:
        logging.error('pep8 check found non-compliant files')  # details on stdout
        return 1
