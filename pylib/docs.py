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
import shutil
import subprocess
import sys

from .utils import BASE_DIR, get_host_platform_name, get_executable_extension
from . import components
from .cmdline import subcmd, Arg


def _get_platform_tools_dir():
    return os.path.join(BASE_DIR, 'tools', get_host_platform_name())


class ExecutableNotAvailable(Exception):
    pass


def _get_executable_from_repo_or_system(name):
    """Get the path of an executable, searching first in the repository's tool directory, then on the system.

    `name` is the base name of the executable without path components and without extensions.

    If no executable with the given name can be found, an ExecutableNotAvailable exception is raised.

    Example:
    - On Windows:
      _get_executable_from_repo_or_system('pandoc') => '.\\tools\\win32\\bin\\pandoc.exe'
    - On Linux with pandoc installed in the system:
      _get_executable_from_repo_or_system('pandoc') => '/usr/bin/pandoc'

    """
    path = os.path.join(_get_platform_tools_dir(), 'bin', name + get_executable_extension())
    if not os.path.exists(path):
        path = shutil.which(name)
        if not path:
            raise ExecutableNotAvailable('Unable to find the executable "{}" in the repository or the system. \
This may be resolved by installing the executable on your system or it may indicate that the RTOS toolchain does not \
support your host platform.'.format(name))
    return path


def _get_package_dirs(required_files=None):
    if required_files is None:
        required_files = set()

    for root, _, files in os.walk(os.path.join(BASE_DIR, 'packages')):
        if required_files.issubset(files):
            yield root


def _get_doc_vars(markdown_file):
    doc_vars = {}
    for line in open(markdown_file).readlines():
        if line.startswith('<!-- %'):
            key, value = line.strip()[6:-4].split(' ', 1)
            doc_vars[key] = value
    return doc_vars


def _build_doc(pkg_dir, top_dir, verbose=False):
    markdown_file = os.path.join(pkg_dir, 'docs.md')
    pdf_file = os.path.join(pkg_dir, 'docs.pdf')
    html_file = os.path.join(pkg_dir, 'docs.html')

    doc_vars = _get_doc_vars(markdown_file)
    if not doc_vars:
        print('Not generating documentation for {} because it is incomplete'.format(pkg_dir))
        return

    css_url = 'docs/stylesheet.css'

    pandoc_executable = _get_executable_from_repo_or_system('pandoc')
    pandoc_cmd = [pandoc_executable,
                  '--write', 'html',
                  '--standalone',
                  '--template=' + os.path.abspath(os.path.join(pkg_dir, 'docs', 'template.html')),
                  '--css=' + css_url,
                  '--toc', '--toc-depth=2'] +\
                 ['-V{}={}'.format(key, value) for key, value in doc_vars.items()] +\
                 ['--output=' + html_file,
                  markdown_file]
    if verbose:
        print(pandoc_cmd)
    subprocess.check_call(pandoc_cmd)

    wkh_executable = _get_executable_from_repo_or_system('wkhtmltopdf')
    wkh_cmd = [wkh_executable,
               '--outline-depth', '2',
               '--page-size', 'A4',
               '--margin-top', '20',
               '--margin-bottom', '25',
               '--margin-left', '20',
               '--margin-right', '20',
               '--header-spacing', '5',
               '--header-html', os.path.abspath(os.path.join(pkg_dir, 'docs', 'header.html')),
               '--footer-spacing', '5',
               '--footer-html', os.path.abspath(os.path.join(pkg_dir, 'docs', 'footer.html')),
               '--replace', 'docid', 'Document ID: {}'.format(doc_vars['docid']),
               html_file,
               pdf_file]
    if verbose:
        print(wkh_cmd)
    try:
        subprocess.check_call(wkh_cmd)
    except subprocess.CalledProcessError:
        if not sys.platform.startswith('win'):
            print('If wkhtmltopdf fails because it cannot connect to an X server, try installing xvfb and running \
your command as xvfb-run -a -s "-screen 0 640x480x16" ./x.py [...]')
        raise


@subcmd(name='docs', cmd='build', help='Build documentation for all variants that support it. \
The generated documentation files are called "docs.pdf" and can be found in each variant\'s package directory.',
        args=(Arg('--verbose', '-v', action='store_true'),))
def build(args):
    components.build(args)
    for pkg_dir in _get_package_dirs(set(('docs.md',))):
        _build_doc(pkg_dir, args.topdir, args.verbose)
    return 0


def is_release_doc_file(filename):
    return 'docs.pdf' in filename


def is_nonrelease_doc_file(filename):
    return 'docs' in filename and 'docs.pdf' not in filename
