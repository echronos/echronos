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
import zipfile
from .utils import top_path, base_path
from .release import _LicenseOpener
from .cmdline import subcmd


@subcmd(name='prj', cmd='build')
def build(args):
    """
    Build a standalone version of the 'prj' tool in the directory prj_build.
    This tool is bundled with the release in the build-release step.
    """
    prj_build_path = top_path(args.topdir, 'prj_build')
    os.makedirs(prj_build_path, exist_ok=True)

    _prj_build(prj_build_path)

    return 0


# pylint: disable=too-many-locals
def _prj_build(output_dir):
    """Create a distributable version of prj.py.

    """
    with zipfile.ZipFile(os.path.join(output_dir, 'prj'), mode='w', compression=zipfile.ZIP_DEFLATED) as zip_file:
        top = os.path.abspath(base_path('prj', 'app'))
        for dir_path, _, file_names in os.walk(top):
            # Exclude temporary files created by the Python interpreter on the fly
            if '__pycache__' in dir_path.lower():
                continue

            archive_dir_path = os.path.relpath(dir_path, top)
            for file_name in file_names:
                # Exclude temporary files created by the Python interpreter on the fly
                if file_name.lower().endswith('.pyc'):
                    continue

                file_path = os.path.join(dir_path, file_name)

                with open(file_path, 'rb') as file_obj:
                    ext = os.path.splitext(file_path)[1]
                    try:
                        agpl_sentinel = _LicenseOpener.agpl_sentinel(ext)
                    except _LicenseOpener.UnknownFiletypeException:
                        agpl_sentinel = None

                    if agpl_sentinel is not None:
                        old_lic_str, sentinel_found, _ = file_obj.peek().decode('utf8').partition(agpl_sentinel)
                        if sentinel_found:
                            old_license_len = len(old_lic_str + sentinel_found)
                            file_obj.read(old_license_len)

                    file_content = file_obj.read()

                if dir_path == top and file_name == 'prj.py':
                    # The python interpreter expects to be informed about the main file in the zip file by naming it
                    # __main__.py
                    archive_file_path = os.path.join(archive_dir_path, '__main__.py')
                else:
                    archive_file_path = os.path.join(archive_dir_path, file_name)
                # Normalize the path to drop unnecessary elements.
                # For example, when the file path starts with "./", this prefix is included in the zip manifest as
                # part of the file name.
                # On Windows, however, this prefix effectively hides the file inside the zip file.
                # Windows and the Python interpreter do not see any files with such a prefix inside a zip file.
                archive_file_path = os.path.normpath(archive_file_path)
                zip_file.writestr(archive_file_path, file_content)
    with open(os.path.join(output_dir, 'prj.bat'), 'w', newline='\r\n') as file_obj:
        file_obj.write('@ECHO OFF\npy -3 %~dp0\\prj %*\n')
    sh_path = os.path.join(output_dir, 'prj.sh')
    with open(sh_path, 'w', newline='\n') as file_obj:
        file_obj.write('''#!/bin/sh
DIR="$(dirname "${0}")"
python3 "${DIR}"/prj "$@"
''')
