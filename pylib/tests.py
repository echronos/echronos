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

import os
import sys
import logging
import unittest
import subprocess
from contextlib import contextmanager
import difflib
import io
import re
from collections import namedtuple
import multiprocessing
import pycodestyle

from .release import _LicenseOpener
from .utils import get_executable_extension, BASE_DIR, find_path, base_to_top_paths, walk, base_path, get_top_dir
from .cmdline import subcmd, Arg


_STD_SUBCMD_ARGS = (
    Arg('tests', metavar='TEST', nargs='*', default=[]),
    Arg('--list', action='store_true', help="List tests (don't execute)", default=False),
    Arg('--verbose', action='store_true', default=False),
    Arg('--quiet', action='store_true', default=False),
)


@subcmd(cmd="test", args=_STD_SUBCMD_ARGS)
def prj(args):
    """Run tests associated with prj modules."""
    with _python_path(os.path.join(BASE_DIR, 'prj', 'app', 'lib')):
        rel_dirs = (os.path.join('external_tools', 'pystache'),
                    os.path.join('prj', 'app'),
                    os.path.join('prj', 'app', 'lib'))

        for rel_dir in rel_dirs:
            for dir_path in base_to_top_paths(args.topdir, rel_dir):
                tests = unittest.TestLoader().discover(dir_path)
                result = unittest.TextTestRunner().run(tests)
                if not result.wasSuccessful():
                    return 2

    return 0


@subcmd(cmd="test", args=_STD_SUBCMD_ARGS)
def x(_):  # pylint: disable=invalid-name
    return unittest.main(module="x_test", argv=[''])


@subcmd(cmd="test")
def pystache(args):
    """Run tests assocaited with pystache modules."""
    sub_env = dict(os.environ)
    sub_env['PYTHONPATH'] = find_path('external_tools', args.topdir)
    return subprocess.call([sys.executable,
                            find_path(os.path.join('external_tools', 'pystache_info', 'test_pystache.py'),
                                      args.topdir)], env=sub_env)


@subcmd(cmd="test", args=_STD_SUBCMD_ARGS)
def units(args):
    """Run rtos unit tests."""
    tests_run = 0
    was_successful = True
    for path in base_to_top_paths(args.topdir, 'unit_tests'):
        result = unittest.main(module=None, argv=['', 'discover', '-s', path]).result
        tests_run += result.testsRun
        was_successful = was_successful and result.wasSuccessful()
    if tests_run > 0 and was_successful:
        return 0
    return 1


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


class _TeamcityReport(pycodestyle.StandardReport):
    """Collect results and print teamcity messages."""

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


@subcmd(cmd="test", help='Run code-style checks against project Python files',
        args=(Arg('--teamcity', action='store_true', help="Provide teamcity output for tests", default=False),
              Arg('--excludes', nargs='*', help="Exclude directories from code-style checks", default=[]),
              Arg('--print-paths', action='store_true', help="Print the paths of files with pylint issues")))
def style(args):
    """Check for PEP8 compliance with the pycodestyle tool and for common coding style conventions via pylint.

    This implements conventions lupHw1 and u1wSS9.
    The enforced maximum line length follows convention TZb0Uv.

    When all project Python files are compliant, this function returns 0.
    When a non-compliant file is found, details about the non-compliance are printed on the standard output stream and
    this function returns 1.
    Runtime errors encountered by the style checker are printed on the standard error stream and raised as the
    appropriate exceptions.

    """
    result = 0
    excludes = ['external_tools', 'tools'] + args.excludes
    # ignored warnings and errors:
    # E402 module level import not at top of file
    codestyle = pycodestyle.StyleGuide(max_line_length=118, paths=[args.topdir], ignore=['E402'], exclude=excludes)
    if args.teamcity:
        codestyle.init_report(_TeamcityReport)
    report = codestyle.check_files()
    if report.total_errors:
        logging.error('Python code-style check found non-compliant files')  # details on stdout
        result = 1
    print('Analyzed {} files with pycodestyle'.format(report.counters['files']))
    if report.counters['files'] < 10:
        logging.error('Analyzed less than 10 files')
        result = 1

    try:
        pylint_result = _run_pylint(excludes, args.print_paths)
    except ImportError as err:
        logging.warning('WARNING: Skipping pylint checks due to ImportError "%s". '
                        'Most likely, the "pylint" Python package is not available. '
                        'Usually, it can be installed with the command "py/python -m pip install pylint"', err)
        pylint_result = 0

    if result == 0:
        result = pylint_result

    return result


def _run_pylint(excludes, print_file_paths=False):
    result = 0

    PylintRun = namedtuple('PylintRun', ('search_paths', 'library_paths'))  # pylint: disable=invalid-name

    pylint_runs = (PylintRun(search_paths=(('', False),
                                           ('pylib', True),
                                           ('unit_tests', True)),
                             library_paths=tuple()),
                   PylintRun(search_paths=(('components', True),
                                           ('packages', True),
                                           (os.path.join('prj', 'app'), True),
                                           (os.path.join('prj', 'example'), True)),
                             library_paths=(os.path.join('prj', 'app'),
                                            os.path.join('prj', 'app', 'lib'),
                                            os.path.join('prj', 'app', 'ply'))))

    analyzed_files = 0
    for pylint_run in pylint_runs:
        file_paths = list(_discover_pylint_file_paths(pylint_run.search_paths, excludes))
        analyzed_files += len(file_paths)
        library_paths = [base_path(library_path) for library_path in pylint_run.library_paths]
        pylint_result = _run_pylint_with_library_paths(file_paths, library_paths, print_file_paths)
        if result == 0:
            result = pylint_result

    print('Analyzed {} files with pylint'.format(analyzed_files))
    if analyzed_files < 10:
        print('Analyzed less than 10 files')
        result = 1

    return result


def _discover_pylint_file_paths(search_paths, excludes):
    def shall_pylint_ignore_path(path):
        return os.path.splitext(path)[1] != '.py' or any([True for exclude in excludes if exclude in path])

    for repo_dir, recurse in search_paths:
        for abs_dir in base_to_top_paths(get_top_dir(), repo_dir):
            if recurse:
                yield from walk(abs_dir, shall_pylint_ignore_path)
            else:
                for name in os.listdir(abs_dir):
                    if not shall_pylint_ignore_path(name):
                        yield os.path.join(abs_dir, name)


def _run_pylint_with_library_paths(file_paths, library_paths, print_file_paths=False):
    result = 0

    with _python_path(*library_paths):
        if print_file_paths:
            for file_path in file_paths:
                file_result = _run_pylint_on_paths((file_path,))
                if result == 0:
                    result = file_result
        else:
            result = _run_pylint_on_paths(file_paths)

    return result


def _run_pylint_on_paths(file_paths):
    # Import pylint here instead of the top of the file so that the rest of the x.py functionality can be used without
    # having to install pylint.
    from pylint.lint import Run
    from pylint.__pkginfo__ import numversion, version

    if numversion[:2] != (1, 8):
        print('WARNING: '
              'The supported version of pylint is 1.8. '
              'The locally installed version of pylint is ' + version + '. '
              'It may report unexpected style violations.')

    if not isinstance(file_paths, list):
        file_paths = list(file_paths)

    runner = Run(['--rcfile=' + base_path('.pylintrc'), '-j', str(_get_number_of_cpus())] + file_paths,
                 exit=False)
    if len(file_paths) == 1 and runner.linter.msg_status != 0:
        print(os.path.relpath(file_paths[0], get_top_dir()) + "\n")

    return runner.linter.msg_status


def _get_number_of_cpus():
    if hasattr(os, 'sched_getaffinity'):
        # pylint: disable=no-member
        cpu_count = len(os.sched_getaffinity(0))
    else:
        try:
            cpu_count = multiprocessing.cpu_count()
        except NotImplementedError:
            cpu_count = 1
    return cpu_count


# pylint: disable=too-many-branches
@subcmd(cmd="test", help='Check that all files have the appropriate license header',
        args=(Arg('--excludes', nargs='*', help="Exclude directories from license header checks", default=[]),))
def licenses(_):
    files_without_license = []
    files_unknown_type = []

    sep = os.path.sep
    if sep == '\\':
        sep = '\\\\'
    # pylint: disable=anomalous-backslash-in-string
    pattern = re.compile('\.git|components{0}.*\.(c|h|xml|md)$|external_tools{0}|pm{0}|\
provenance{0}|out{0}|release{0}|prj_build|tools{0}|docs{0}manual_template|packages{0}[^{0}]+{0}rtos-|\
.*__pycache__|x_test_data{0}.*\.md|x_test_data{0}tasks{0}.*'.format(sep))
    for dirpath, _, files in os.walk(BASE_DIR):
        for file_name in files:
            path = os.path.join(dirpath, file_name)
            rel_path = os.path.relpath(path, BASE_DIR)
            if not pattern.match(rel_path):
                # expect shell-style comment format for .pylintrc
                if rel_path == '.pylintrc':
                    license_sentinel = _LicenseOpener.license_sentinel('.sh')
                else:
                    ext = os.path.splitext(file_name)[1]
                    try:
                        license_sentinel = _LicenseOpener.license_sentinel(ext)
                    except _LicenseOpener.UnknownFiletypeException:
                        files_unknown_type.append(path)
                        continue

                if license_sentinel is not None:
                    with open(path, 'rb') as file_obj:
                        _, sentinel_found, _ = file_obj.peek().decode('utf8').partition(license_sentinel)
                        if not sentinel_found:
                            files_without_license.append(path)

    if files_without_license:
        logging.error('License check found files without a license header:')
        for file_path in files_without_license:
            logging.error('    %s', file_path)

    if files_unknown_type:
        logging.error('License check found files of unknown type:')
        for file_path in files_unknown_type:
            logging.error('    %s', file_path)
        return 1

    if files_without_license:
        return 1

    return 0


# pylint: disable=too-many-branches
@subcmd(cmd="test", help='Check that all files belonging to external tools map 1-1 with provenance listings')
def provenance(args):
    exemptions = [os.path.join(BASE_DIR, 'tools', 'LICENSE.md'),
                  os.path.join(BASE_DIR, 'external_tools', 'LICENSE.md')]
    files_nonexistent = []
    files_not_listed = []
    files_listed = []

    # Check that all files in provenance FILES listings exist.
    for provenance_path in base_to_top_paths(args.topdir, 'provenance'):
        for dirpath, subdirs, files in os.walk(provenance_path):
            for list_path in [os.path.join(dirpath, file_name) for file_name in files if file_name == 'FILES']:
                with open(list_path) as file_obj:
                    for line in file_obj:
                        file_path = line.strip()
                        file_abs_path = os.path.normpath(os.path.join(os.path.dirname(provenance_path), file_path))
                        if os.path.exists(file_abs_path):
                            files_listed.append(file_abs_path)
                        else:
                            files_nonexistent.append((file_path, list_path))

    # Check that all files in 'external_tools' and 'tools' are listed in a provenance FILES listing.
    for target_dir in base_to_top_paths(args.topdir, ('tools', 'external_tools')):
        for dirpath, subdirs, files in os.walk(target_dir):
            # Exempt any __pycache__ dirs from the check
            if '__pycache__' in subdirs:
                subdirs.remove('__pycache__')

            # Exempt tools/share/xyz from the check.
            # This directory contains xyz-generated provenance information including file listings with paths relative
            # to the 'tools' directory, sometimes including other files in tools/share/xyz, so we leave them here to
            # preserve their paths and put a note in the relevant ORIGIN files to refer here for more info.
            if dirpath == os.path.abspath(os.path.join(BASE_DIR, 'tools', 'share')) and 'xyz' in subdirs:
                subdirs.remove('xyz')

            for file_path in [os.path.normpath(os.path.join(dirpath, file_name)) for file_name in files]:
                if file_path not in files_listed + exemptions:
                    files_not_listed.append(file_path)

    # Log all results and return 1 if there were any problematic cases
    if files_nonexistent:
        logging.error('Provenance check found files listed that don\'t exist:')
        for file_path, list_path in files_nonexistent:
            logging.error('    %s (listed in %s)', file_path, list_path)

    if files_not_listed:
        logging.error('Provenance check found files without provenance information:')
        for file_path in files_not_listed:
            logging.error('    %s', file_path)
        return 1

    if files_nonexistent:
        return 1

    return 0


@subcmd(cmd="test", help='Run system tests, i.e., tests that check the behavior of full RTOS systems. \
This command supports the same options as the Python unittest framework.')
def systems(args):
    tests_run = 0
    was_successful = True
    for path in base_to_top_paths(args.topdir, 'packages'):
        result = unittest.main(module=None, argv=['', 'discover', '-s', path, '-t', os.path.dirname(path)]).result
        tests_run += result.testsRun
        was_successful = was_successful and result.wasSuccessful()
    if tests_run > 0 and was_successful:
        return 0
    return 1


class GdbTestCase(unittest.TestCase):
    """A Pythonic interface to running an RTOS system executable against a GDB command file and checking whether the
    output produced matches a given reference output.

    The external interface of this class is that of unittest.TestCase to be accessed by the unittest frameworks.

    To use this class for new tests, subclass this class in a Python file under the packages/ directory and set the
    prx_path attribute.
    """
    prx_path = None

    def __init__(self, *args, **kwargs):
        self.gdb_output = None
        super().__init__(*args, **kwargs)

    def setUp(self):
        assert os.path.exists(self.prx_path), self.prx_path
        assert os.path.isabs(self.prx_path), self.prx_path

        self.gdb_commands_path = os.path.splitext(self.prx_path)[0] + '.gdb'

        parent_packages_path = os.path.join(self.prx_path.rpartition(os.sep + 'packages' + os.sep)[0], 'packages')
        self.search_paths = [parent_packages_path]

        rel_prx_path = os.path.relpath(self.prx_path, parent_packages_path)
        self.system_name = os.path.splitext(rel_prx_path)[0].replace(os.sep, '.')

        rel_executable_path = os.path.join('out', self.system_name.replace('.', os.sep), self._get_executable_name())
        self.executable_path = os.path.abspath(rel_executable_path)

        self._build()

    def _get_executable_name(self):  # pylint: disable=no-self-use
        return 'system' + get_executable_extension()

    def test(self):
        assert os.path.exists(self.executable_path)
        assert os.path.exists(self.gdb_commands_path)
        test_output = self._get_test_output()
        reference_output = self._get_reference_output()
        if test_output != reference_output:
            new_reference_path = os.path.splitext(self.prx_path)[0] + '.gdboutnew'
            with open(new_reference_path, 'wb') as file_obj:
                file_obj.write(self.gdb_output)
            sys.stdout.write('System test failed:\n\t{}\n\t{}\n\t{}\n'.format(self.gdb_commands_path,
                                                                              self.executable_path,
                                                                              new_reference_path))
            for line in difflib.unified_diff(reference_output.splitlines(), test_output.splitlines(),
                                             'reference', 'test'):
                sys.stdout.write(line + '\n')
        assert test_output == reference_output

    def _build(self):
        subprocess.check_call([sys.executable, os.path.join(BASE_DIR, 'prj', 'app', 'prj.py')] +
                              ['--search-path={}'.format(sp) for sp in self.search_paths] +
                              ['build', self.system_name])

    def _get_test_output(self):
        test_command = self._get_test_command()
        # A timeout is necessary to make the test fail if the test target hangs.
        # A 30 second timeout is sufficient for all current test systems.
        self.gdb_output = subprocess.check_output(test_command, timeout=30)
        # for an unknown reason, decode() handles Windows line breaks incorrectly so convert them to UNIX linebreaks
        output_str = self.gdb_output.replace(b'\r\n', b'\n').decode()
        return self._filter_gdb_output(output_str)

    def _get_test_command(self):
        return ('gdb', '--batch', self.executable_path, '-x', self.gdb_commands_path)

    def _get_reference_output(self):
        reference_path = os.path.splitext(self.prx_path)[0] + '.gdbout'
        with open(reference_path) as file_obj:
            reference_output = file_obj.read()
        return self._filter_gdb_output(reference_output)

    @staticmethod
    def _filter_gdb_output(gdb_output):
        # pylint: disable=anomalous-backslash-in-string
        delete_patterns = (re.compile('^(\[New Thread .+)$'),)
        replace_patterns = (re.compile('Breakpoint [0-9]+ at (0x[0-9a-f]+): file (.+), line ([0-9]+)'),
                            re.compile('^Breakpoint .* at (.+)$'),
                            re.compile('Breakpoint [0-9]+, (0x[0-9a-f]+) in'),
                            re.compile('( <__register_frame_info\+[0-9a-f]+>)'),
                            re.compile('=(0x[0-9a-f]+)'),
                            re.compile('Inferior( [0-9]+ )\[process( [0-9]+\]) will be killed'),
                            re.compile('^([0-9]+\t.+)$'),
                            re.compile('^entry \(\) at (.+)$'),
                            re.compile('^(Thread [0-9]+ "[^"]+" hit )'),
                            re.compile('^(Thread [0-9]+ hit )Breakpoint '))
        filtered_result = io.StringIO()
        for line in gdb_output.splitlines(True):
            match = None
            for pattern in delete_patterns:
                match = pattern.search(line)
                if match:
                    break
            if match:
                continue
            for pattern in replace_patterns:
                while True:
                    match = pattern.search(line)
                    if match:
                        for group in match.groups():
                            line = line.replace(group, '')
                    else:
                        break
            filtered_result.write(line)
        return filtered_result.getvalue()


class PpcQemuTestCase(GdbTestCase):
    @unittest.skipIf(os.name == 'nt', "not supported on this operating system because cross-platform toolchain is not\
 available")
    def setUp(self):
        super().setUp()
        self.qemu = subprocess.Popen(('qemu-system-ppc', '-S', '-nographic', '-gdb', 'tcp::18181', '-M', 'ppce500',
                                      '-kernel', self.executable_path))

    def _get_test_command(self):
        return ('powerpc-linux-gdb', '--batch', self.executable_path, '-x', self.gdb_commands_path)

    def tearDown(self):
        self.qemu.terminate()
        self.qemu.wait()


class Armv7mQemuTestCase(GdbTestCase):
    @unittest.skipIf(os.name == 'nt', "not supported on this operating system because cross-platform toolchain is not\
 available")
    def setUp(self):
        super().setUp()

        # After gdb disconnects from qemu it will execute ridiculously fast and print lots of text.
        # Prevent this from happening by piping qemu's stdout to /dev/null
        self.fnull = open(os.devnull, 'w')

        # The LM3S6965 is arbitrarily chosen due to it's simple memory model
        # (Non-aliased XIP ROM @ 0x00000000)
        qemu_command = ('qemu-system-arm', '-s', '-S', '-M', 'lm3s6965evb', '-nographic',
                        '-semihosting', '-kernel', self.executable_path)

        self.qemu = subprocess.Popen(qemu_command, stdout=self.fnull)

    def _get_test_command(self):
        return ('arm-none-eabi-gdb', '--batch', self.executable_path, '-x', self.gdb_commands_path)

    def tearDown(self):
        self.qemu.terminate()
        self.qemu.wait()
        self.fnull.close()
