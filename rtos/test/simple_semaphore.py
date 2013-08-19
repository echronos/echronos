import os
import ctypes
from rtos import sched


class testSimpleSemaphore:
    @classmethod
    def setUpClass(cls):
        r = os.system("./prj/app/prj.py build posix.unittest.simple-semaphore")
        system = "out/posix/unittest/simple-semaphore/system"
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sem = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_semaphores')[0]

    def test_simple(self):
        import sys

        self.impl.pub_sem_init()

        # Try wait all the semaphore, should all fail
        for i in range(10):
            assert self.impl.rtos_sem_try_wait(i) == 0

        # Now signal all semaphores
        for i in range(10):
            self.impl.rtos_sem_post(i)

        # Now try-locking should pass on all
        for i in range(10):
            assert self.impl.rtos_sem_try_wait(i) == 1

        # If we try again, it should all fail.
        for i in range(10):
            assert self.impl.rtos_sem_try_wait(i) == 0

        # Now post all
        for i in range(10):
            self.impl.rtos_sem_post(i)
        for i in range(10):
            self.impl.rtos_sem_wait(i)
        for i in range(10):
            self.impl.rtos_sem_post(i)

        # Check that block/unblock is called correctly.
        #
        # This code is a little bit icky. It would be nice if it could
        # be improved.
        #
        expected_blocks = 20
        self.impl.rtos_sem_wait(0)

        block_calls = 0
        unblocked = None

        BlockFuncPtr = ctypes.CFUNCTYPE(None)
        UnblockFuncPtr = ctypes.CFUNCTYPE(None, ctypes.c_byte)
        GetCurrentTaskPtr = ctypes.CFUNCTYPE(ctypes.c_byte)

        def unblock_func(task_id):
            nonlocal unblocked
            unblocked = task_id
        unblock_func_ptr = UnblockFuncPtr(unblock_func)

        def block_func():
            nonlocal block_calls
            block_calls += 1
            if block_calls == expected_blocks:
                self.impl.rtos_sem_post(0)
        block_func_ptr = BlockFuncPtr(block_func)

        def get_current_task():
            return 5
        get_current_task_ptr = GetCurrentTaskPtr(get_current_task)

        self.impl.pub_set_block_ptr(block_func_ptr)
        self.impl.pub_set_unblock_ptr(unblock_func_ptr)
        self.impl.pub_set_get_current_task_ptr(get_current_task_ptr)

        self.impl.rtos_sem_wait(0)

        assert block_calls == expected_blocks
        assert unblocked == 5

    def test_counting(self):
        self.impl.pub_sem_init()

        assert self.impl.rtos_sem_try_wait(0) == 0

        for i in range(10):
            self.impl.rtos_sem_post(0)

        for i in range(10):
            assert self.impl.rtos_sem_try_wait(0) == 1

        assert self.impl.rtos_sem_try_wait(0) == 0
