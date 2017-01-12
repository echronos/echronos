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

import ctypes
import os
import sys

from rtos import sched
from pylib.utils import get_executable_extension


class testSimpleMutex:
    @classmethod
    def setUpClass(cls):
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.simple-mutex")
        system = "out/posix/unittest/simple-mutex/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_mutex = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_mutexes')[0]

    def test_simple(self):
        # Try lock all the mutex, should all pass
        for i in range(10):
            assert self.impl.rtos_mutex_try_lock(i) == 1
        # Now try-locking should fail on all
        for i in range(10):
            assert self.impl.rtos_mutex_try_lock(i) == 0
        # If we unlock, and try again, it should all succeed.
        for i in range(10):
            self.impl.rtos_mutex_unlock(i)
        for i in range(10):
            assert self.impl.rtos_mutex_try_lock(i) == 1
        # Now unlock, and try and use regular lock
        for i in range(10):
            self.impl.rtos_mutex_unlock(i)
        for i in range(10):
            self.impl.rtos_mutex_lock(i)
        for i in range(10):
            self.impl.rtos_mutex_unlock(i)

        # Check that yield is called correctly.
        #
        # This code is a little bit icky. It would be nice if it could
        # be improved.
        #
        expected_yields = 20
        self.impl.rtos_mutex_lock(0)

        YieldFuncPtr = ctypes.CFUNCTYPE(None)
        yield_calls = 0

        def yield_func():
            nonlocal yield_calls
            yield_calls += 1
            if yield_calls == expected_yields:
                self.impl.rtos_mutex_unlock(0)

        yield_func = YieldFuncPtr(yield_func)
        self.impl.pub_set_yield_ptr(yield_func)

        self.impl.rtos_mutex_lock(0)
        assert yield_calls == expected_yields
