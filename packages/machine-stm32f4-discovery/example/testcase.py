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
import subprocess
import unittest
import sys
from pylib import tests


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


class TestCase(tests.GdbTestCase):
    @unittest.skipIf(os.name == 'nt', "not supported on this operating system because cross-platform toolchain is not\
 available")
    def setUp(self):
        super(TestCase, self).setUp()

        # After gdb disconnects from qemu it will execute ridiculously fast and print lots of text.
        # Prevent this from happening by piping qemu's stdout/stderr to /dev/null
        FNULL = open(os.devnull, 'w')

        args = ('xvfb-run', '-a', 'exec', 'echronos-qemu-system-arm', '-s', '-S', '-mcu', 'STM32F407VG',
                '-semihosting', '-kernel', self.executable_path, stdout=FNULL, stderr=subprocess.STDOUT)

        self.qemu = subprocess.Popen(args)

    def _get_test_command(self):
        return ('arm-none-eabi-gdb', '--batch', self.executable_path, '-x', self.gdb_commands_path)

    def tearDown(self):
        self.qemu.terminate()
        self.qemu.wait()
