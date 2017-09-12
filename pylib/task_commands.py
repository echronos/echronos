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

from .cmdline import cmd, Arg
from .task import Task

_OFFLINE_ARG = Arg('-o', '--offline', action='store_true',
                   help='Skip all git commands that require an Internet connection')
_TASKNAME_ARG = Arg('taskname', nargs='?', help='The name of the task to manage. Defaults to the active git branch.')


@cmd(args=(_OFFLINE_ARG, Arg('taskname', help='The name of the task to manage.')),
     help='Developers: create a new task to work on, including a template for the task description.')
def create(task_cfg, args):
    task = Task(task_cfg, name=args.taskname, checkout=False)
    task.create(offline=args.offline)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG),
     help='Developers: bring a task up-to-date with the latest changes on the mainline branch. '
          'If the task is not yet on review, this rebases the task branch onto the mainline branch. '
          'If the task is on review, the mainline changes are merged into the task branch.')
def update(task_cfg, args):
    task = Task(task_cfg, name=args.taskname)
    task.update(offline=args.offline)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG),
     help='Developers: request reviews for a task.')
def request_reviews(task_cfg, args):
    task = Task(task_cfg, name=args.taskname)
    task.request_reviews(args.offline)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG,
           Arg('-a', '--accept', action='store_true',
               help='Create and complete the review with the conclusion "accepted", commit, and push it.')),
     help='Reviewers: create a stub for a new review of the active task branch.')
def review(task_cfg, args):
    task = Task(task_cfg, name=args.taskname)
    task.review(offline=args.offline, accept=args.accept)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG),
     help='Reviewers: create, commit, and push a new review for the active task branch with the conclusion '
          '"Accepted". This is an alias for the command "x.py task review --accept".')
def accept(task_cfg, args):
    args.accept = True
    review(task_cfg, args)


@cmd(help='Developers: integrate a completed task branch into the mainline branch.',
     args=(_OFFLINE_ARG, _TASKNAME_ARG))
def integrate(task_cfg, args):
    task = Task(task_cfg, name=args.taskname)
    task.integrate(offline=args.offline)
