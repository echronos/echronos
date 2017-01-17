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


class testRrSched:  # pylint: disable=invalid-name
    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.sched-rr")
        system = "out/posix/unittest/sched-rr/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sched = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            assert self.impl.pub_sched_get_next() == model.get_next()
            assert self.impl_sched == model

        states = sched.RrSchedModel.states(10, assume_runnable=True)
        for idx, state in enumerate(states):
            yield "check_state.{}".format(idx), check_state, state


class testPrioSched:  # pylint: disable=invalid-name
    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.sched-prio")
        system = "out/posix/unittest/sched-prio/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sched = ctypes.POINTER(sched.get_prio_sched_struct(10)).in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            assert self.impl.pub_sched_get_next() == model.get_next()
            assert self.impl_sched == model

        states = sched.PrioSchedModel.states(10, assume_runnable=True)
        for idx, state in enumerate(states):
            yield "check_state.{}".format(idx), check_state, state


class testPrioInheritSched:  # pylint: disable=invalid-name
    test_size = 5

    @classmethod
    def setUpClass(cls):  # pylint: disable=invalid-name
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.sched-prio-inherit")
        system = "out/posix/unittest/sched-prio-inherit/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        pub_sched_tasks = ctypes.POINTER(sched.get_prio_inherit_sched_struct(cls.test_size))
        cls.impl_sched = pub_sched_tasks.in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            impl_next = self.impl.pub_sched_get_next()
            if impl_next == 255:
                impl_next = None
            model_next = model.get_next()
            assert impl_next == model_next, "Result: {} Expected: {}".format(impl_next, model_next)
            assert self.impl_sched == model

        states = sched.PrioInheritSchedModel.states(self.test_size, assume_runnable=True)
        for idx, state in enumerate(states):
            yield "check_state.{}".format(idx), check_state, state
