import os
import ctypes
from rtos import sched


class testSimpleMutex:
    @classmethod
    def setUpClass(cls):
        r = os.system("./prj/app/prj.py build posix.unittest.simple-mutex")
        system = "out/posix/unittest/simple-mutex/system"
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_mutex = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_mutexes')[0]

    def test_simple(self):
        import sys

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
