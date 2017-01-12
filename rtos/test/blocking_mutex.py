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

NUM_MUTEXES = 10


BlockOnFuncPtr = ctypes.CFUNCTYPE(None, ctypes.c_ubyte)
UnblockFuncPtr = ctypes.CFUNCTYPE(None, ctypes.c_ubyte)
GetCurrentTaskPtr = ctypes.CFUNCTYPE(ctypes.c_ubyte)


class MutexStruct(ctypes.Structure):
    _fields_ = [("holder", ctypes.c_uint8)]


class testBlockingMutex:
    @classmethod
    def setUpClass(cls):
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.blocking-mutex")
        system = "out/posix/unittest/blocking-mutex/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_mutex = (ctypes.POINTER(MutexStruct * NUM_MUTEXES)).in_dll(cls.impl, 'pub_mutexes')[0]

    # pylint: disable=no-self-argument
    def set_unblock_func(cls, fn):
        cls.unblock_func_ptr = UnblockFuncPtr(fn)
        cls.impl.pub_set_unblock_ptr(cls.unblock_func_ptr)

    # pylint: disable=no-self-argument
    def set_block_on_func(cls, fn):
        cls.block_on_func_ptr = BlockOnFuncPtr(fn)
        cls.impl.pub_set_block_on_ptr(cls.block_on_func_ptr)

    # pylint: disable=no-self-argument
    def set_get_current_task_func(cls, fn):
        cls.get_current_task_ptr = GetCurrentTaskPtr(fn)
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
