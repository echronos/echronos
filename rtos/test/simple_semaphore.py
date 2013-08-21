import os
import ctypes
from rtos import sched

NUM_SEMAPHORES = 10
ALL_SEMAPHORES = list(range(NUM_SEMAPHORES))
SEM_ID_NONE = 255
SEM_VALUE_ZERO = 0

NUM_TASKS = 10
ALL_TASKS = list(range(NUM_TASKS))

BlockFuncPtr = ctypes.CFUNCTYPE(None)
UnblockFuncPtr = ctypes.CFUNCTYPE(None, ctypes.c_ubyte)
GetCurrentTaskPtr = ctypes.CFUNCTYPE(ctypes.c_ubyte)


class SemaphoreStruct(ctypes.Structure):
    _fields_ = [("value", ctypes.c_ubyte)]


class SemaphoreTest:
    @classmethod
    def setUpClass(cls):
        r = os.system("./prj/app/prj.py build posix.unittest.simple-semaphore")
        system = "out/posix/unittest/simple-semaphore/system"
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sem = ctypes.POINTER(SemaphoreStruct).in_dll(cls.impl, 'pub_semaphores')
        cls.impl_waiters = ctypes.POINTER(ctypes.c_ubyte).in_dll(cls.impl, 'pub_waiters')
        cls.impl.rtos_sem_try_wait.res_type = ctypes.c_bool


class testSimpleSemaphoreWhiteBox(SemaphoreTest):
    """Semaphore tests (white-box)."""
    def test_count(self):
        self.impl.pub_sem_init()

        post_count = 256

        # Check all semaphore start at value zero.
        for sem_id in ALL_SEMAPHORES:
            assert self.impl_sem[sem_id].value == SEM_VALUE_ZERO

        for test_sem_id in ALL_SEMAPHORES:
            # Check that on posting the value is incremented (only
            # on the single semaphore).
            for i in range(1, post_count):
                self.impl.rtos_sem_post(test_sem_id)
                for sem_id in ALL_SEMAPHORES:
                    assert self.impl_sem[sem_id].value == (i if sem_id == test_sem_id else SEM_VALUE_ZERO)

            # Now check that wait correctly decrements
            for i in reversed(range(0, post_count - 1)):
                self.impl.rtos_sem_wait(test_sem_id)
                for sem_id in ALL_SEMAPHORES:
                    assert self.impl_sem[sem_id].value == (i if sem_id == test_sem_id else SEM_VALUE_ZERO)

    def test_unblock_multiple(self):
        current_task_id = None
        current_sem_id = None

        def get_current_task():
            return current_task_id
        get_current_task_ptr = GetCurrentTaskPtr(get_current_task)
        self.impl.pub_set_get_current_task_ptr(get_current_task_ptr)

        def block_func():
            for task_id in ALL_TASKS:
                assert self.impl_waiters[task_id] == current_sem_id if task_id == current_task_id else SEM_ID_NONE
            self.impl.rtos_sem_post(current_sem_id)
        block_func_ptr = BlockFuncPtr(block_func)
        self.impl.pub_set_block_ptr(block_func_ptr)

        for current_task_id in ALL_TASKS:
            for current_sem_id in ALL_SEMAPHORES:
                self.impl.rtos_sem_wait(current_sem_id)
                for task_id in ALL_TASKS:
                    assert self.impl_waiters[task_id] == SEM_ID_NONE

class testSimpleSemaphore(SemaphoreTest):
    """Semaphore tests (black-box)."""

    def test_simple(self):
        """Simple test of semaphore functionality."""
        self.impl.pub_sem_init()

        # Try wait all the semaphores
        # all should fail
        for sem_id in ALL_SEMAPHORES:
            assert not self.impl.rtos_sem_try_wait(sem_id)

        # Post once to all semaphores
        for sem_id in ALL_SEMAPHORES:
            self.impl.rtos_sem_post(sem_id)

        # Now waiting on all semaphores should succeed
        for sem_id in ALL_SEMAPHORES:
            assert self.impl.rtos_sem_try_wait(sem_id)

        # Trying to wait again should fail on all.
        for sem_id in ALL_SEMAPHORES:
            assert not self.impl.rtos_sem_try_wait(sem_id)

    def test_block_unblock(self):
        # Check that block/unblock is called correctly.
        #
        # This code is a little bit icky. It would be nice if it could
        # be improved.
        #
        self.impl.pub_sem_init()

        task_id = 5
        expected_blocks = 20

        block_calls = 0
        unblocked = None

        def unblock_func(task_id):
            nonlocal unblocked
            unblocked = task_id
        unblock_func_ptr = UnblockFuncPtr(unblock_func)
        self.impl.pub_set_unblock_ptr(unblock_func_ptr)

        def block_func():
            nonlocal block_calls
            block_calls += 1
            if block_calls == expected_blocks:
                self.impl.rtos_sem_post(0)
        block_func_ptr = BlockFuncPtr(block_func)
        self.impl.pub_set_block_ptr(block_func_ptr)

        def get_current_task():
            return task_id
        get_current_task_ptr = GetCurrentTaskPtr(get_current_task)
        self.impl.pub_set_get_current_task_ptr(get_current_task_ptr)

        self.impl.rtos_sem_wait(0)

        assert block_calls == expected_blocks
        assert unblocked == task_id

    def test_counting(self):
        self.impl.pub_sem_init()

        counter = 200

        assert self.impl.rtos_sem_try_wait(0) == 0

        for i in range(200):
            self.impl.rtos_sem_post(0)

        for i in range(200):
            assert self.impl.rtos_sem_try_wait(0) == 1

        assert self.impl.rtos_sem_try_wait(0) == 0
