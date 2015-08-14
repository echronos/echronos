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
import ice
import signal
import zipfile
from .utils import get_host_platform_name, top_path, chdir, base_path
from .release import _LicenseOpener
from .cmdline import subcmd


@subcmd(name='prj', cmd='build')
def build(args):
    """
    Build a standalone version of the 'prj' tool in the directory
    prj_build_<host>. This tool is bundled with the release in the
    build-release step.
    """
    host = get_host_platform_name()

    prj_build_path = top_path(args.topdir, 'prj_build_{}'.format(host))
    os.makedirs(prj_build_path, exist_ok=True)

    if sys.platform == 'win32':
        _prj_build_win32(prj_build_path)
    else:
        _prj_build_unix(prj_build_path, host)


def _prj_build_unix(output_dir, host):
    if sys.platform == 'darwin':
        extras = ['-framework', 'CoreFoundation', '-lz']
    elif sys.platform == 'linux':
        extras = ['-lz', '-lm', '-lpthread', '-lrt', '-ldl', '-lcrypt', '-lutil']
    else:
        print("Building prj currently unsupported on {}".format(sys.platform))
        return 1

    prj_app_path = base_path('prj', 'app')
    tools_path = base_path('tools')
    with chdir(output_dir):
        ice.create_lib('prj', prj_app_path, main='prj')
        ice.create_lib('prjlib', os.path.join(prj_app_path, 'lib'))
        ice.create_lib('pystache', os.path.join(prj_app_path, 'pystache'),
                       excluded=['setup', 'pystache.tests', 'pystache.commands'])
        ice.create_lib('ply', os.path.join(prj_app_path, 'ply'), excluded=['setup'])
        ice.create_stdlib()
        ice.create_app(['stdlib', 'prj', 'prjlib', 'pystache', 'ply'])

        cmd = ['gcc', '*.c', '-o', 'prj', '-I{}/include/python3.3m/'.format(tools_path),
               '-I{}/{}/include/python3.3m/'.format(tools_path, host),
               '-L{}/{}/lib/python3.3/config-3.3m'.format(tools_path, host),
               '-lpython3.3m']
        cmd += extras

        cmd = ' '.join(cmd)
        r = os.system(cmd)
        if r != 0:
            print("Error building {}. cmd={}. ".format(_show_exit(r), cmd))


def _show_exit(exit_code):
    sig_num = exit_code & 0xff
    exit_status = exit_code >> 8
    if sig_num == 0:
        return "exit: {}".format(exit_status)
    else:
        _SIG_NAMES = {k: v for v, k in signal.__dict__.items() if v.startswith('SIG')}
        return "signal: {}".format(_SIG_NAMES.get(sig_num, 'Unknown signal {}'.format(sig_num)))


def _prj_build_win32(output_dir):
    """Create a distributable version of prj.py.

    We currently do not have the infrastructure in place to statically compile and link prj.py and its dependencies
    against the complete python interpreter.

    However, it is still desirable to create only a single resource that can stand alone given an installed python
    interpreter.
    Therefore, collect prj and its dependencies in a zip file that is executable by the python interpreter.

    """
    with zipfile.ZipFile(os.path.join(output_dir, 'prj'), mode='w') as zip_file:
        top = os.path.abspath(base_path('prj', 'app'))
        for dir_path, _, file_names in os.walk(top):
            archive_dir_path = os.path.relpath(dir_path, top)
            for file_name in file_names:
                file_path = os.path.join(dir_path, file_name)

                with open(file_path, 'rb') as f:
                    ext = os.path.splitext(file_path)[1]
                    try:
                        agpl_sentinel = _LicenseOpener._agpl_sentinel(ext)
                    except _LicenseOpener.UnknownFiletypeException:
                        agpl_sentinel = None

                    if agpl_sentinel is not None:
                        old_lic_str, sentinel_found, _ = f.peek().decode('utf8').partition(agpl_sentinel)
                        if sentinel_found:
                            old_license_len = len(old_lic_str + sentinel_found)
                            f.read(old_license_len)

                    file_content = f.read()

                if dir_path == top and file_name == 'prj.py':
                    # The python interpreter expects to be informed about the main file in the zip file by naming it
                    # __main__.py
                    archive_file_path = os.path.join(archive_dir_path, '__main__.py')
                else:
                    archive_file_path = os.path.join(archive_dir_path, file_name)
                zip_file.writestr(archive_file_path, file_content)
    with open(os.path.join(output_dir, 'prj.bat'), 'w') as f:
        f.write('@ECHO OFF\npython %~dp0\\prj')
