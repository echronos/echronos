import os
import ctypes
from rtos import sched

NUM_SEMAPHORES = 10
ALL_SEMAPHORES = list(range(NUM_SEMAPHORES))

class testSimpleSemaphore:
    @classmethod
    def setUpClass(cls):
        r = os.system("./prj/app/prj.py build posix.unittest.simple-semaphore")
        system = "out/posix/unittest/simple-semaphore/system"
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sem = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_semaphores')[0]
        cls.impl.rtos_sem_try_wait.res_type = ctypes.c_bool

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

        UnblockFuncPtr = ctypes.CFUNCTYPE(None, ctypes.c_byte)
        def unblock_func(task_id):
            nonlocal unblocked
            unblocked = task_id
        unblock_func_ptr = UnblockFuncPtr(unblock_func)
        self.impl.pub_set_unblock_ptr(unblock_func_ptr)

        BlockFuncPtr = ctypes.CFUNCTYPE(None)
        def block_func():
            nonlocal block_calls
            block_calls += 1
            if block_calls == expected_blocks:
                self.impl.rtos_sem_post(0)
        block_func_ptr = BlockFuncPtr(block_func)
        self.impl.pub_set_block_ptr(block_func_ptr)

        GetCurrentTaskPtr = ctypes.CFUNCTYPE(ctypes.c_byte)
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
