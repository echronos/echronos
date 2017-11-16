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
import subprocess
import unittest
from pylib import tests


class TestCase(tests.GdbTestCase):
    @unittest.skipIf(os.name == 'nt', "not supported on this operating system because cross-platform toolchain is not\
 available")
    def setUp(self):
        super(TestCase, self).setUp()

        # After gdb disconnects from qemu it will execute ridiculously fast and print lots of text.
        # Prevent this from happening by piping qemu's stdout/stderr to /dev/null
        self.fnull = open(os.devnull, 'w')

        qemu_command = ('xvfb-run', '-a', 'exec', 'echronos-qemu-system-arm', '-s', '-S', '-mcu', 'STM32F407VG',
                        '-semihosting', '-kernel', self.executable_path)

        self.qemu = subprocess.Popen(qemu_command, stdout=self.fnull, stderr=subprocess.STDOUT)

    def _get_test_command(self):
        return ('arm-none-eabi-gdb', '--batch', self.executable_path, '-x', self.gdb_commands_path)

    def tearDown(self):
        self.qemu.terminate()
        self.qemu.wait()
        self.fnull.close()
