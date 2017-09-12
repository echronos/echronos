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
import sys
import unittest

from unit_tests import sched
from pylib.utils import get_executable_extension, base_path


class TestSimpleMutex(unittest.TestCase):
    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        result = os.system("{} {} build posix.unittest.simple-mutex"
                           .format(sys.executable, base_path('prj', 'app', 'prj.py')))
        system = "out/posix/unittest/simple-mutex/system" + get_executable_extension()
        assert result == 0
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

        yield_func_ptr = ctypes.CFUNCTYPE(None)
        yield_calls = 0

        def yield_func():
            nonlocal yield_calls
            yield_calls += 1
            if yield_calls == expected_yields:
                self.impl.rtos_mutex_unlock(0)

        self.impl.pub_set_yield_ptr(yield_func_ptr(yield_func))

        self.impl.rtos_mutex_lock(0)
        assert yield_calls == expected_yields
