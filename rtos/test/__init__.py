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

__all__ = ['sched', 'simple_mutex', 'blocking_mutex', 'simple_semaphore']

import ctypes
import os
import random
import sys

from pylib.utils import get_executable_extension


class testSimple:  # pylint: disable=invalid-name
    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        exit_code = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.simple")
        system = "out/posix/unittest/simple/system" + get_executable_extension()
        assert exit_code == 0
        cls.simple = ctypes.CDLL(system)

    def test_foo(self):
        assert self.simple.foo() == 37

    def test_bar(self):
        def check_add(sum_x, sum_y):
            assert sum_x + sum_y == self.simple.add(sum_x, sum_y)

        rand = random.Random()
        rand.seed(37)

        for idx in range(20):
            int_x = rand.randint(0, 5000)
            int_y = rand.randint(0, 5000)
            yield "test_bar{}".format(idx), check_add, int_x, int_y
