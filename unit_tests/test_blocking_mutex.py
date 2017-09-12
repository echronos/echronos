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

from pylib.utils import get_executable_extension, base_path

NUM_MUTEXES = 10


class MutexStruct(ctypes.Structure):
    _fields_ = [("holder", ctypes.c_uint8)]


class TestBlockingMutex(unittest.TestCase):
    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        prj_path = base_path('prj', 'app', 'prj.py')
        command = "build posix.unittest.blocking-mutex"
        result = os.system("{} {} {}".format(sys.executable, prj_path, command))
        system = "out/posix/unittest/blocking-mutex/system" + get_executable_extension()
        assert result == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_mutex = (ctypes.POINTER(MutexStruct * NUM_MUTEXES)).in_dll(cls.impl, 'pub_mutexes')[0]

    # pylint: disable=no-self-argument
    def set_unblock_func(cls, function):
        # pylint: disable=attribute-defined-outside-init
        cls.unblock_func_ptr = ctypes.CFUNCTYPE(None, ctypes.c_ubyte)(function)
        cls.impl.pub_set_unblock_ptr(cls.unblock_func_ptr)

    # pylint: disable=no-self-argument
    def set_block_on_func(cls, function):
        # pylint: disable=attribute-defined-outside-init
        cls.block_on_func_ptr = ctypes.CFUNCTYPE(None, ctypes.c_ubyte)(function)
        cls.impl.pub_set_block_on_ptr(cls.block_on_func_ptr)

    # pylint: disable=no-self-argument
    def set_get_current_task_func(cls, function):
        # pylint: disable=attribute-defined-outside-init
        cls.get_current_task_ptr = ctypes.CFUNCTYPE(ctypes.c_ubyte)(function)
        cls.impl.pub_set_get_current_task_ptr(cls.get_current_task_ptr)

    def test_simple(self):
        self.impl.pub_mutex_init()

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

    def test_block(self):
        # Check that block is called correctly.
        #
        # This code is a little bit icky. It would be nice if it could
        # be improved.
        #
        self.impl.pub_mutex_init()

        current_task = 1
        blocked_on = None

        def get_current_task():
            return current_task

        self.set_get_current_task_func(get_current_task)

        def block_on(task_id):
            nonlocal blocked_on
            blocked_on = task_id
            self.impl.rtos_mutex_unlock(1)

        self.set_block_on_func(block_on)

        # As task 1, lock mutex 1
        current_task = 1
        self.impl.rtos_mutex_lock(1)

        # As task 2, lock mutex 1, should call block_on with task_id == 1
        current_task = 2
        self.impl.rtos_mutex_lock(1)

        assert blocked_on == 1
        assert self.impl_mutex[1].holder == 2
