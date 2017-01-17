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
import os.path
from .utils import TOP_DIR, BASE_DIR
from .cmdline import cmd, Arg
from .task import Task, TaskConfiguration

TASK_CFG = TaskConfiguration(repo_path=TOP_DIR,
                             tasks_path=os.path.join('pm', 'tasks'),
                             description_template_path=os.path.join(BASE_DIR, '.github', 'PULL_REQUEST_TEMPLATE.md'),
                             reviews_path=os.path.join('pm', 'reviews'),
                             mainline_branch='master')

_OFFLINE_ARG = Arg('-o', '--offline', action='store_true',
                   help='Skip all git commands that require an Internet connection')
_TASKNAME_ARG = Arg('taskname', nargs='?', help='The name of the task to manage. Defaults to the active git branch.')


@cmd(args=(_OFFLINE_ARG, Arg('taskname', help='The name of the task to manage.')),
     help='Developers: create a new task to work on, including a template for the task description.')
def create(args):
    task = Task(TASK_CFG, name=args.taskname, checkout=False)
    task.create(offline=args.offline)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG),
     help='Developers: bring a task up-to-date with the latest changes on the mainline branch "{}". '
          'If the task is not yet on review, this rebases the task branch onto the mainline branch. '
          'If the task is on review, the mainline changes are merged into the task branch.'
     .format(TASK_CFG.mainline_branch))
def update(args):
    task = Task(TASK_CFG, name=args.taskname)
    task.update(offline=args.offline)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG),
     help='Developers: request reviews for a task.')
def request_reviews(args):
    task = Task(TASK_CFG, name=args.taskname)
    task.request_reviews(args.offline)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG,
           Arg('-a', '--accept', action='store_true',
               help='Create and complete the review with the conclusion "accepted", commit, and push it.')),
     help='Reviewers: create a stub for a new review of the active task branch.')
def review(args):
    task = Task(TASK_CFG, name=args.taskname)
    task.review(offline=args.offline, accept=args.accept)


@cmd(args=(_OFFLINE_ARG, _TASKNAME_ARG),
     help='Reviewers: create, commit, and push a new review for the active task branch with the conclusion '
          '"Accepted". This is an alias for the command "x.py task review --accept".')
def accept(args):
    args.accept = True
    review(args)


@cmd(help='Developers: integrate a completed task branch into the mainline branch {}.'
     .format(TASK_CFG.mainline_branch),
     args=(_TASKNAME_ARG,))
def integrate(args):
    task = Task(TASK_CFG, name=args.taskname)
    task.integrate()
