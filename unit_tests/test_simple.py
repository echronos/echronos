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

import ctypes
import os
import random
import sys
import unittest

from pylib.utils import get_executable_extension, base_path


class TestSimple(unittest.TestCase):
    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        result = os.system("{} {} build posix.unittest.simple"
                           .format(sys.executable, base_path('prj', 'app', 'prj.py')))
        system = "out/posix/unittest/simple/system" + get_executable_extension()
        assert result == 0
        cls.simple = ctypes.CDLL(system)

    def test_foo(self):
        assert self.simple.foo() == 37

    def test_bar(self):
        rand = random.Random()
        rand.seed(37)

        for _ in range(20):
            int_x = rand.randint(0, 5000)
            int_y = rand.randint(0, 5000)
            self.assertEqual(int_x + int_y, self.simple.add(int_x, int_y))
