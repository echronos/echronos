import os
import sys
import shutil
import tempfile
import calendar
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


def base_to_top_paths(topdir, *path):
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
