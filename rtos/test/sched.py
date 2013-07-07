import os
import ctypes
from rtos import sched


class testRrSched:
    @classmethod
    def setUpClass(cls):
        r = os.system("./prj/app/prj.py build posix.unittest.sched-rr")
        system = "out/posix/unittest/sched-rr/system"
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sched = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            assert self.impl.pub_sched_get_next() == model.get_next()
            assert self.impl_sched == model

        states = sched.RrSchedModel.states(10, assume_runnable=True)
        for i, s in enumerate(states):
            yield "check_state.{}".format(i), check_state, s
