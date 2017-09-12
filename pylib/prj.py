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
                        license_sentinel = _LicenseOpener.license_sentinel(ext)
                    except _LicenseOpener.UnknownFiletypeException:
                        license_sentinel = None

                    if license_sentinel is not None:
                        old_lic_str, sentinel_found, _ = file_obj.peek().decode('utf8').partition(license_sentinel)
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

        for third_party_package in ('pystache', 'ply'):
            _add_3rd_party_package_to_zip(third_party_package, zip_file)

    with open(os.path.join(output_dir, 'prj.bat'), 'w', newline='\r\n') as file_obj:
        file_obj.write('@ECHO OFF\npy -3 %~dp0\\prj %*\n')
    sh_path = os.path.join(output_dir, 'prj.sh')
    with open(sh_path, 'w', newline='\n') as file_obj:
        file_obj.write('''#!/bin/sh
DIR="$(dirname "${0}")"
python3 "${DIR}"/prj "$@"
''')


def _add_3rd_party_package_to_zip(third_party_package, zip_file):
    third_party_dir_path = base_path('external_tools')
    for dir_path, _, file_names in os.walk(os.path.join(third_party_dir_path, third_party_package)):
        # Exclude temporary files created by the Python interpreter and test files
        if '__pycache__' in dir_path.lower() or 'tests' in dir_path:
            continue

        archive_dir_path = os.path.relpath(dir_path, third_party_dir_path)
        for file_name in file_names:
            # Exclude temporary files created by the Python interpreter on the fly
            if file_name.lower().endswith('.pyc'):
                continue

            with open(os.path.join(dir_path, file_name), 'rb') as file_obj:
                file_content = file_obj.read()

            archive_file_path = os.path.join(archive_dir_path, file_name)
            archive_file_path = os.path.normpath(archive_file_path)
            zip_file.writestr(archive_file_path, file_content)
