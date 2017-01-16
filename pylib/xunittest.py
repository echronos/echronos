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

import re
import sys
import inspect
import unittest
import importlib
import functools


def ispackage(obj):
    """Return true if the object is a package."""
    return inspect.ismodule(obj) and hasattr(obj, '__path__')


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
    if isinstance(testcase, unittest.suite._ErrorHolder):  # pylint: disable=protected-access
        return str(testcase)
    return testcase.testcase_name


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
                testcase.testcase_name = "{}.{}.{}".format(cls.__module__, cls.__name__, name)
                yield testcase
            else:
                if inspect.isgeneratorfunction(method):
                    gen = getattr(cls(), name)
                    for gen_name, *test in gen():
                        f = functools.partial(*test)
                        testcase = MethodTestCase(f, cls)
                        # pylint: disable=attribute-defined-outside-init
                        testcase.testcase_name = "{}.{}.{}".format(cls.__module__, cls.__name__, gen_name)
                        yield testcase
                else:
                    testcase = MethodTestCase(getattr(cls(), name), cls)
                    # pylint: disable=attribute-defined-outside-init
                    testcase.testcase_name = "{}.{}.{}".format(cls.__module__, cls.__name__, name)
                    yield testcase


def discover_tests_module(module):
    """Generate testcases from the module."""
    for name, obj in [(name, getattr(module, name)) for name in dir(module)]:
        if inspect.isclass(obj) and (issubclass(obj, unittest.TestCase) or re.match('test.*', name)):
            yield from discover_tests_class(obj)
        elif callable(obj) and re.match('test_.*', name):
            if inspect.isgeneratorfunction(obj):
                for gen_name, *test in obj():
                    f = functools.partial(*test)
                    testcase = unittest.FunctionTestCase(f)
                    testcase.testcase_name = "{}.{}:{}".format(obj.__module__, obj.__name__, gen_name)
                    yield testcase
            else:
                testcase = unittest.FunctionTestCase(obj)
                testcase.testcase_name = "{}.{}".format(obj.__module__, obj.__name__)
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
