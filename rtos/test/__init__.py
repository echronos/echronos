__all__ = ['sched', 'simple_mutex', 'blocking_mutex', 'simple_semaphore']

import os
import ctypes
import unittest
import random


class testSimple:
    @classmethod
    def setUpClass(cls):
        r = os.system("./prj/app/prj.py build posix.unittest.simple")
        system = "out/posix/unittest/simple/system"
        assert r == 0
        cls.simple = ctypes.CDLL(system)

    def test_foo(self):
        assert self.simple.foo() == 37

    def test_bar(self):
        def check_add(x, y):
            assert x + y == self.simple.add(x, y)

        rand = random.Random()
        rand.seed(37)

        for i in range(20):
            x = rand.randint(0, 5000)
            y = rand.randint(0, 5000)
            yield "test_bar{}".format(i), check_add, x, y
