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

"""
A model of the RTOS schedulers in Python.

This can be used for testing purposes to ensure implementation matches the model.

"""
from itertools import product
import ctypes

_RTOS_TASK_ID_NONE = 0xFF


def head(iter_):
    """Return the first item in an iterator, or None if the iterator is empty."""
    try:
        return next(iter_)
    except StopIteration:
        return None


def rotate(sequence, num):
    """Rotate a sequence by num positions.

    Note: This function returns a new list; it does not mutate the seqeunce.

    """
    return sequence[num:] + sequence[:num]


def incmod(i, num):
    return (i + 1) % num


def get_rr_sched_struct(num_tasks):
    """Return an implementation mock for a round-robin scheduler with 'num_tasks' tasks."""
    class RrSchedTaskStruct(ctypes.Structure):
        _fields_ = [("runnable", ctypes.c_bool)]

    class RrSchedStruct(ctypes.Structure):
        _fields_ = [("cur", ctypes.c_uint8),
                    ("tasks", RrSchedTaskStruct * num_tasks)]

        def __init__(self, *rargs, **kwargs):
            self.cur = None
            super().__init__(*rargs, **kwargs)

        def __str__(self):
            run_state = ''.join(['X' if x.runnable else ' ' for x in self.tasks])
            return "<RrSchedImpl cur={} runnable=[{}]".format(self.cur, run_state)

        def __eq__(self, model):
            if model.cur != self.cur:
                return False
            for idx, runnable in model.indexed:
                if self.tasks[idx].runnable != runnable:
                    return False
            return True

        def set(self, model):
            self.cur = model.cur
            for idx, runnable in model.indexed:
                self.tasks[idx].runnable = runnable
            assert self == model
    return RrSchedStruct


def get_prio_sched_struct(num_tasks):
    """Return an implementation mock for a priority scheduler with 'num_tasks' tasks."""
    class PrioSchedTaskStruct(ctypes.Structure):
        _fields_ = [("runnable", ctypes.c_bool)]

    class PrioSchedStruct(ctypes.Structure):
        _fields_ = [("tasks", PrioSchedTaskStruct * num_tasks)]

        def __str__(self):
            run_state = ''.join(['X' if x.runnable else ' ' for x in self.tasks])
            return "<PrioSchedImpl runnable=[{}]".format(run_state)

        def __eq__(self, model):
            for idx, runnable in model.indexed:
                if self.tasks[idx].runnable != runnable:
                    return False
            return True

        def set(self, model):
            for idx, runnable in model.indexed:
                self.tasks[idx].runnable = runnable
            assert self == model
    return PrioSchedStruct


def get_prio_inherit_sched_struct(num_tasks):
    """Return an implementation mock for a prioity inheritance scheduler with 'num_tasks' tasks."""
    class PrioInheritTaskStruct(ctypes.Structure):
        _fields_ = [("blocked_on", ctypes.c_uint8)]

    class PrioInheritSchedStruct(ctypes.Structure):
        _fields_ = [("tasks", PrioInheritTaskStruct * num_tasks)]

        def __str__(self):
            blocked_on = ''.join(['{:d}'.format(x.blocked_on) if x.blocked_on is not _RTOS_TASK_ID_NONE else '.'
                                  for x in self.tasks])
            return "<PrioInheritSchedImpl blocked_on=[{}]".format(blocked_on)

        def __eq__(self, model):
            for idx, result in enumerate(model.blocked_on):
                if result is None:
                    result = _RTOS_TASK_ID_NONE
                if self.tasks[idx].blocked_on != result:
                    return False
            return True

        def set(self, model):
            for idx, blocked_on in enumerate(model.blocked_on):
                if blocked_on is None:
                    blocked_on = _RTOS_TASK_ID_NONE
                self.tasks[idx].blocked_on = blocked_on
            assert self == model
    return PrioInheritSchedStruct


class BaseSchedModel:
    def __init__(self, runnable):
        self.runnable = runnable
        self.size = len(runnable)

    @property
    def indexed(self):
        """Return the runnable list as tuples (index, runnable)."""
        return list(enumerate(self.runnable))

    @property
    def runnable_str(self):
        return ''.join(['X' if runnable else ' ' for runnable in self.runnable])

    # pylint: disable=no-self-use
    def get_next(self):
        """Based on the scheduling algorithm return the next task to run.

        This method may mutate the scheduler object.

        This method should be implemented by a sub-class.

        """
        raise Exception("get_next should be implemented by sub-class.")


class RrSchedModel(BaseSchedModel):
    """A model of the round-robin scheduler."""

    def __init__(self, runnable, cur):
        super().__init__(runnable)
        self.cur = cur

    def __str__(self):
        return '<RrSched cur={} runnable=[{}]>'.format(self.cur, self.runnable_str)

    def get_next(self):
        next_index = head(idx for idx, runnable in rotate(self.indexed, incmod(self.cur, self.size)) if runnable)
        self.cur = self.size - 1 if next_index is None else next_index
        return next_index

    @classmethod
    def states(cls, num, assume_runnable=False):
        """Return all possible round-robin scheduler states for num tasks.

        If assume_runnable is True then only include states where at least one task is runnable.

        """
        objects = (cls(*state) for state in product(product((True, False), repeat=num), range(num)))
        return filter(lambda state: any(state.runnable), objects) if assume_runnable else objects


class PrioSchedModel(BaseSchedModel):
    """A model of the strict priority scheduler."""

    def __str__(self):
        return '<PrioSched runnable=[{}]>'.format(self.runnable_str)

    def get_next(self):
        return head(idx for idx, runnable in self.indexed if runnable)

    @classmethod
    def states(cls, num, assume_runnable=False):
        """Return all possible priority scheduler states for num tasks.

        If assume_runnable is True then only include states where at least one task is runnable.

        """
        objects = (cls(state) for state in product((True, False), repeat=num))
        return filter(lambda state: any(state.runnable), objects) if assume_runnable else objects


class PrioInheritSchedModel(BaseSchedModel):
    """A model of the strict priority with inheritance scheduler."""

    def __init__(self, blocked_on):
        super().__init__(runnable=[])
        self.blocked_on = blocked_on

    @property
    def blocked_on_str(self):
        return ''.join(['{:d}'.format(runnable) if runnable is not None else '.' for runnable in self.blocked_on])

    @property
    def any_runnable(self):
        return any(idx == val for idx, val in enumerate(self.blocked_on))

    def __str__(self):
        return '<PrioInheritSched blocked_on=[{}]>'.format(self.blocked_on_str)

    def get_next(self):
        def resolve_block_chain(task_id):
            seen = set()
            while True:
                blocked_on = self.blocked_on[task_id]
                assert blocked_on not in seen
                if blocked_on in (task_id, None):
                    return blocked_on
                else:
                    seen.add(task_id)
                    task_id = blocked_on
            return None

        return head(task_id for
                    task_id in map(resolve_block_chain, range(len(self.blocked_on)))
                    if task_id is not None)

    @classmethod
    def states(cls, num, assume_runnable=False):
        """Return all possible priority scheduler states for num tasks.

        If assume_runnable is True then only include states where at least one task is runnable.

        """
        def check_blocked(blocked_list, task_id):
            seen = set()
            while True:
                blocked_on = blocked_list[task_id]
                if blocked_on in seen:
                    return False
                elif blocked_on in (task_id, None):
                    return True
                else:
                    seen.add(task_id)
                    task_id = blocked_on
            return False

        def check_blocked_list(blocked_list):
            result = all(check_blocked(blocked_list, task_id) for task_id in range(len(blocked_list)))
            return result
        objects = (cls(state) for state in product(list(range(num)) + [None], repeat=num)
                   if check_blocked_list(state))
        return filter(lambda state: state.any_runnable, objects) if assume_runnable else objects


def main():
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('--assume-runnable', action='store_true',
                        help="Ensure schedulers have at least 1 runnable task.")
    parser.add_argument('--sched', choices=['prio', 'rr'], required=True)
    parser.add_argument('N', type=int,
                        help="Number of tasks in scheduler.")
    args = parser.parse_args()

    sched_class = {'prio': PrioSchedModel, 'rr': RrSchedModel}[args.sched]

    for state in sched_class.states(args.N, args.assume_runnable):
        before = str(state)
        next_ = state.get_next()
        after = str(state)
        print("{:>5}  {}  {} ".format(next_, before, after))

if __name__ == '__main__':
    main()
