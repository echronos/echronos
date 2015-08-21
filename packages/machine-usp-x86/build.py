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

from collections import namedtuple
import os.path
import re
import subprocess
from prj import execute, SystemBuildError


schema = {
    'type': 'dict',
    'name': 'module',
    'dict_type': ([{'type': 'string', 'name': 'output_type', 'default': 'executable'}], [])
}


def run(system, configuration=None):
    return system_build(system, configuration)


def system_build(system, configuration):
    inc_path_args = ['-I%s' % i for i in system.include_paths]

    if len(system.c_files) == 0:
        raise SystemBuildError("Zero C files in system definition")

    shared_args = ['-shared', '-fPIC'] if configuration['output_type'] == 'shared-library' else []

    execute(['gcc', '-o', system.output_file, '-Wall', '-Werror', '-Wextra', '-ggdb', '-m32'] +
            _get_gcc_options() + shared_args + inc_path_args + system.asm_files + system.c_files)


def _get_gcc_options():
    options = []
    version = _get_gcc_version()
    if version.major > 4 or (version.major == 4 and version.minor > 7):
        options.append('-Wpedantic')
    return options


def _get_gcc_version():
    gcc_output = subprocess.check_output(['gcc', '--version']).decode()
    first_line = gcc_output.splitlines()[0]
    version_str = re.search(' ([0-9]+\.[0-9])+\.[0-9]+$', first_line).group(1)
    version_tuple = map(int, version_str.split('.'))
    return namedtuple('Version', 'major minor')(*version_tuple)
