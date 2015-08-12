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

import os
import sys
import shutil
import tempfile
import calendar
import subprocess
from contextlib import contextmanager


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


BASE_DIR = os.path.normpath(os.path.join(os.path.dirname(follow_link(__file__)), ".."))
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
    else:
        raise IOError("Unable to find the relative path '{}' in the repository hierarchy".format(path))


def un_base_path(path):
    """Reverse the operation performed by `base_path`.

    For all `x`, `un_base_path(base_path(x)) == x`.
    """
    if BASE_DIR == '':
        return path
    else:
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
    elif sys.platform == 'linux':
        return 'x86_64-unknown-linux-gnu'
    elif sys.platform == 'win32':
        return 'win32'
    else:
        raise RuntimeError('Unsupported platform {}'.format(sys.platform))


_executable_extension = None


def get_executable_extension():
    global _executable_extension
    if _executable_extension is None:
        _executable_extension = {'darwin': '',
                                 'linux': '',
                                 'win32': '.exe',
                                 }[sys.platform]
    return _executable_extension


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

    def convert_paths(self, paths):
        """
        Convert a single path or a list of paths so that they are safe to pass as command line parameters to git.
        This is necessary to account for differences in how git binaries handle paths across platforms.
        In particular, when combining a native Python interpreter with a cygwin git binary on Windows, all paths
        passed to git need to be relative and have Unix instead of Windows path separators.

        """
        assert isinstance(paths, (str, list))
        make_relative = lambda path: os.path.relpath(path, self.local_repository) if os.path.isabs(path) else path
        convert = lambda path: make_relative(path).replace(os.sep, self.sep)
        if isinstance(paths, str):
            return convert(paths)
        else:
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
        return self._do(['mv'] + self.convert_paths(src_list) + [self.convert_paths(dst)])

    def add(self, files):
        """Add the list of files to the index in preparation of a future commit."""
        return self._do(['add'] + self.convert_paths(files))

    def commit(self, msg, files=None):
        """Commit the changes in the specified 'files' with the given 'message' to the currently active branch.

        If 'files' is None (or unspecified), all staged files are committed.

        """
        assert isinstance(msg, str)
        assert files is None or isinstance(files, list)
        if files is None:
            file_args = []
        else:
            file_args = self.convert_paths(files)
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
