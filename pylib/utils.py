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

# pylint: disable=too-many-public-methods
import os
import sys
import shutil
import tempfile
import calendar
import subprocess
import traceback
from collections import namedtuple
from contextlib import contextmanager


def follow_link(link):
    """Return the underlying file from a symbolic link.

    This will (recursively) follow links until a non-symbolic link is found.
    No cycle checks are performed by this function.

    """
    if not os.path.islink(link):
        return link
    path = os.readlink(link)
    if os.path.isabs(path):
        return path
    return follow_link(os.path.join(os.path.dirname(link), path))


# Directory containing the core repository
BASE_DIR = os.path.normpath(os.path.join(os.path.dirname(follow_link(__file__)), ".."))
# Top directory from which x.py (or a wrapper in the case of a client repository) was invoked
TOP_DIR = os.path.dirname(os.path.normpath(follow_link(os.path.abspath(traceback.extract_stack()[0][0]))))
BASE_TIME = calendar.timegm((2013, 1, 1, 0, 0, 0, 0, 0, 0))


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


def top_path(topdir, *path):
    """Return a path relative to the directory in which the x tool or wrapper was invoked.

    This function is equivalent to base_path(), except when the x tool is invoked in a client repository through a
    wrapper.
    In that case, the specified path is not appended to the directory containing the core x.py file, but the directory
    containing the wrapper x.py file invoked by the user.

    """
    return os.path.join(topdir, *path)


def base_to_top_paths(topdir, paths, only_existing=True):
    """For each directory from BASE_DIR up to topdir in the directory tree, append the specified path(s) and return
    the resulting sequence.

    Examples:
    Assume the directory structure /rtos/core/components
                                              packages
                                        /packages
    and BASE_DIR = '/rtos/core/'
    - base_to_top_paths('/rtos/', 'packages', only_existing=False)
        => iterator('/rtos/core/packages', '/rtos/packages')
    - base_to_top_paths('/rtos/', ['packages'], only_existing=False)
        => iterator('/rtos/core/packages', '/rtos/packages')
    - base_to_top_paths('/rtos/', ['components', 'packages'], only_existing=False)
        => iterator('/rtos/core/components', '/rtos/core/packages', '/rtos/components', '/rtos/packages')
    - base_to_top_paths('/rtos/', ['foo', 'components', 'packages'])
        => iterator('/rtos/core/components', '/rtos/core/packages', '/rtos/packages')

    - BASE_DIR = '/rtos/'; base_to_top_paths('/rtos/', 'packages')
        => iterator('/rtos/packages')

    """
    if isinstance(paths, str):
        paths = (paths,)

    cur_dir = os.path.abspath(BASE_DIR)
    stop_dir = os.path.abspath(topdir)
    iterate = True
    while iterate:
        for path in paths:
            full_path = os.path.join(cur_dir, path)
            if not only_existing or os.path.exists(full_path):
                yield full_path
        iterate = (cur_dir != stop_dir)
        cur_dir = os.path.dirname(cur_dir)


def find_path(path, topdir):
    """Find a file in a top to bottom search through the repository hierarchy.

    For all repositories/directories from `topdir` down to the core repository in BASE_DIR, check whether the relative
    `path` exists and, if yes, return its absolute path.
    `path` can be any file system object, including links and directories.
    If `path` exists in multiple repositories in the repository hierarchy, the top-most one is returned.
    If `path` does not exist anywhere in the repository hierarchy, this function raises an IOError exception.

    Assume the following directory layout:
    /foo/
    /foo/x
    /foo/core/
    /foo/core/x
    /foo/core/y
    /foo/bar/baz

    BASE_DIR is /foo/core

    find_path('x', '/foo') -> '/foo/x'
    find_path('y', '/foo') -> '/foo/core/y'
    find_path('z', '/foo') -> IOError
    find_path('baz', '/foo') -> IOError
    find_path('bar/baz', '/foo') -> '/foo/bar/baz'

    """
    paths = list(base_to_top_paths(topdir, path))
    if paths:
        return paths[-1]
    raise IOError("Unable to find the relative path '{}' in the repository hierarchy".format(path))


def un_base_path(path):
    """Reverse the operation performed by `base_path`.

    For all `x`, `un_base_path(base_path(x)) == x`.
    """
    if BASE_DIR == '':
        return path
    return path[len(BASE_DIR) + 1:]


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
    def always_true(_):
        return True

    if flt is None:
        flt = always_true

    file_list = []
    for root, _, files in os.walk(path):
        file_list.extend([os.path.join(root, f) for f in files if not flt(os.path.join(root, f))])
    return file_list


def get_host_platform_name():
    if sys.platform == 'darwin':
        return 'x86_64-apple-darwin'
    if sys.platform == 'linux':
        return 'x86_64-unknown-linux-gnu'
    if sys.platform == 'win32':
        return 'win32'
    raise RuntimeError('Unsupported platform {}'.format(sys.platform))


_EXECUTABLE_EXTENSION = None


def get_executable_extension():
    global _EXECUTABLE_EXTENSION  # pylint: disable=global-statement
    if _EXECUTABLE_EXTENSION is None:
        _EXECUTABLE_EXTENSION = {'darwin': '',
                                 'linux': '',
                                 'win32': '.exe'}[sys.platform]
    return _EXECUTABLE_EXTENSION


# pylint: disable=too-many-public-methods
class Git:
    """
    Represents common state applicable to a series of git invocations and provides a pythonic interface to git.
    """
    def __init__(self, local_repository=os.getcwd()):
        """
        Create a Git instance with which all commands operate with the given local repository.
        """
        assert isinstance(local_repository, str)
        # Check whether the local repository is a directory managed by git.
        # Note that '.git' can be a directory in case of a git repository or a file in case of a git submodule
        assert os.path.exists(os.path.join(local_repository, '.git'))
        self.local_repository = local_repository
        self._sep = None
        self._branches = None
        self._remote_branches = None

    def convert_paths(self, paths):
        """
        Convert a single path or a list of paths so that they are safe to pass as command line parameters to git.
        This is necessary to account for differences in how git binaries handle paths across platforms.
        In particular, when combining a native Python interpreter with a cygwin git binary on Windows, all paths
        passed to git need to be relative and have Unix instead of Windows path separators.

        """
        assert isinstance(paths, (str, list))

        def convert(path):
            if os.path.isabs(path):
                path = os.path.relpath(path, self.local_repository)
            return path.replace(os.sep, self.sep)

        if isinstance(paths, str):
            return convert(paths)
        return [convert(path) for path in paths]

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
        output = self._do(['ls-tree', '-r', '--name-only', 'HEAD:pm'])
        for line in output.splitlines():
            if line.startswith('reviews'):
                line = line.replace('reviews', '', 1)
                return line[0]
        raise LookupError('git ls-tree does not list any files in pm/reviews as expected')

    def _do(self, parameters, as_lines=False):
        """
        Execute the git command line tool with the given command-line parameters and return the console output as a
        string.
        """
        assert isinstance(parameters, list)
        raw_data = subprocess.check_output(['git'] + parameters, cwd=self.local_repository).decode()
        if as_lines:
            return raw_data.splitlines()
        return raw_data

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

    def branch_hash(self, branch=None):
        """Return the hash of the latest commit on a given branch as a UNIX timestamp.

        The branch may be ommitted, in which case it defaults to the current head.

        """
        return self._log_pretty('%H', branch=branch)

    def working_dir_clean(self):
        """Return True is the working directory is clean."""
        return self._do(['status', '--porcelain']) == ''


Remote = namedtuple('Remote', ('name', 'url'))

_TOP_DIR = None


def get_top_dir():
    """Return the absolute path of the directory in which the x tool or wrapper was invoked.

    Take, for example, the following arrangement of a client and a core repository:
    /client-repo/
    /client-repo/x.py (wrapper for x.py in core submodule)
    /client-repo/core/ (git submodule containing the RTOS repository)
    /client-repo/core/x.py (x.py from RTOS repository)
    /foo/ => /client-repo/ (a symlink called /foo that refers to /client-repo)
    When executing /client-repo/x.py, this function returns '/client-repo'.
    When executing /foo/x.py, this function returns '/client-repo'.
    When executing /client-repo/core/x.py, this function returns '/client-repo/core'.
    When executing ../x.py from the current working directory /client-repo/core/, this function returns '/client-repo'

    """
    global _TOP_DIR  # pylint: disable=global-statement
    if _TOP_DIR is None:
        stack_frames = traceback.extract_stack()
        top_stack_frame = stack_frames[0]
        relative_file_path = top_stack_frame[0]
        absolute_file_path = _sanitize_path(relative_file_path)
        _TOP_DIR = os.path.dirname(absolute_file_path)
    return _TOP_DIR


def _sanitize_path(path):
    """Converts an arbitrary valid path to an normalized, absolute path free of symbolic link indirections.

    For example, take the following file system layout:
    /foo/
    /foolink => /foo/
    /foo/bar
    /foo/barlink => /foolink/bar

    If the current working directory is '/foolink':
    - _sanitize_path('bar') => '/foo/bar'
    - _sanitize_path('barlink') => '/foo/bar'
    - _sanitize_path('.//../foo//barlink') => '/foo/bar'

    """
    return os.path.normpath(follow_link(os.path.abspath(path)))
