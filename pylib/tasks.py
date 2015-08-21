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

import os
import re
import time
import string
import datetime
import subprocess
from random import choice
from .utils import Git
from .cmdline import subcmd, Arg


def _task_dir(topdir, *args):
    return os.path.join(topdir, 'pm', 'tasks', *args)


def _review_dir(topdir, *args):
    return os.path.join(topdir, 'pm', 'reviews', *args)


@subcmd(cmd="task", help='Generate a random 6-char alphanumeric string')
def tag(_):
    tag_length = 6
    tag_chars = string.ascii_letters + string.digits
    return ''.join(choice(tag_chars) for _ in range(tag_length))


_review_template = """RTOS Task Review
=======================

Task name: %(branch)s
Version reviewed: %(sha)s
Reviewer: %(reviewer)s
Date: %(date)s
Conclusion: Accepted/Rework

Overall comments:


Specific comments
=================

Location: filename:linenum
Comment:

Location: filename:linenum
Comment:
"""


@subcmd(cmd="task", args=(Arg('reviewers', metavar='REVIEWER', nargs='+'),))
def review(args):
    """Create a new review for the current branch."""
    # Check the directory is clean
    status = subprocess.check_output(['git', 'status', '--porcelain'], cwd=args.topdir)
    if status != b'':
        print("Can't commit while directory is dirty. Aborting.")
        return 1

    branch = subprocess.check_output(['git', 'symbolic-ref', 'HEAD'], cwd=args.topdir).decode().strip().split('/')[-1]
    review_dir = _review_dir(args.topdir, branch)

    sha = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=args.topdir).decode().strip()

    if os.path.exists(review_dir):
        review_number = sorted([int(re.match(r'review-([0-9]+)..*', f).group(1))
                                for f in os.listdir(review_dir)])[-1] + 1
    else:
        review_number = 0

    msg = "Creating review [%d] for branch [%s] with reviewers %s: (y/n) " % (review_number, branch, args.reviewers)
    x = input(msg)
    if x != 'y':
        print("aborted")
        return 1

    date = datetime.datetime.now().strftime('%Y-%m-%d')
    os.makedirs(review_dir, exist_ok=True)
    params = {
        'branch': branch,
        'sha': sha,
        'date': date
    }
    review_files = []
    for reviewer in args.reviewers:
        review_fn = os.path.join(review_dir, 'review-%d.%s' % (review_number, reviewer))
        review_files.append(review_fn)
        with open(review_fn, 'w', newline='\n') as f:
            params['reviewer'] = reviewer
            f.write(_review_template % params)

    git = Git(local_repository=args.topdir)
    # now, git add, git commit -m <msg> and git push.
    msg = 'Review request {} for {}'.format(review_number, branch)
    git.add(review_files)
    git.commit(msg)
    git.push(branch, branch)


_task_template = """Task: {}
==============================================================================

Motivation
----------


Goals
--------


Test Plan
---------

"""


@subcmd(cmd="task",
        args=(Arg('taskname', metavar='TASKNAME'),
              Arg('--no-fetch', dest='fetch', action='store_false', default='true')))
def create(args):
    remote = 'origin'
    branch_from = remote + '/development'

    git = Git(local_repository=args.topdir)
    if not git.working_dir_clean():
        print("Working directory must be clean before creating a new task.")
        return 1

    if args.fetch:
        # Ensure that we have the very last origin/development to branch
        # from.
        git.fetch()

    fullname = tag(None) + '-' + args.taskname
    git.branch(fullname, branch_from, track=False)
    git.push(fullname, fullname, set_upstream=True)
    git.checkout(fullname)

    task_fn = _task_dir(args.topdir, fullname)
    with open(task_fn, 'w', newline='\n') as f:
        f.write(_task_template.format(fullname))

    print("Edit file: {} then add/commit/push.".format(task_fn))
    print('Suggest committing as: git commit -m "New task: {}"'.format(fullname))


class _Review:
    """
    Represents a review on a development task/branch.
    """
    def __init__(self, file_path):
        """
        Create a _Review instance representing the review in the given 'file_path'.
        This provides easy access to the review's review round, author, and conclusion.
        """
        assert isinstance(file_path, str)
        assert os.path.isfile(file_path)
        self.file_path = file_path
        basename = os.path.basename(file_path)
        round_author = basename[7:]
        round, author = round_author.split('.', maxsplit=1)
        self.round = int(round)
        self.author = author
        self._conclusion = None

    def _get_conclusion(self):
        """
        Determine the conclusion of this review.
        The conclusion can be expected to be one of 'Accepted/Rework' (i.e., the review has not been completed),
        'Accepted', or 'Rework'.
        """
        f = open(self.file_path)
        for line in f:
            if line.startswith('Conclusion: '):
                conclusion = line.split(':')[1].strip()
                f.close()
                return conclusion.lower()
        f.close()
        assert False

    @property
    def conclusion(self):
        """
        Return the conclusion of this review.
        The conclusion can be expected to be one of 'Accepted/Rework' (i.e., the review has not been completed),
        'Accepted', or 'Rework'.
        """
        if self._conclusion is None:
            self._conclusion = self._get_conclusion()
        return self._conclusion

    def is_accepted(self):
        return self.conclusion in ['accept', 'accepted']

    def is_rework(self):
        return self.conclusion == 'rework'

    def is_done(self):
        return self.conclusion != 'accepted/rework'


class _InvalidTaskStateError(RuntimeError):
    """
    To be raised when a task state transition cannot be performed because the task is not in the appropriate source
    state.
    """
    pass


class _Task:
    """
    Represents a development task.
    Each task is associated with several task-related resources, such as the corresponding git branch, a description
    file, and reviews.
    Over its life time, a task traverses several states (such as ready for implementation, complete and ready for
    integration, and integrated).
    This class implements some aspects of task management and helps automating state transitions.
    """
    @staticmethod
    def create(name=None, top_directory=None):
        """
        Create and return a new _Task instance, falling back to defaults if the optional task name and repository
        directory are not specified.
        If 'top_directory' is not specified, it defaults to the current working directory which must be a valid local
        git repository.
        If 'name' is not specified, it defaults to the name of the active branch in the task's top directory.
        """
        if top_directory is None:
            top_directory = os.getcwd()
        # Note that '.git' can be a directory in case of a git repository or a file in case of a git submodule
        assert os.path.exists(os.path.join(os.getcwd(), '.git'))
        if name is None:
            # derive name from current git branch
            name = Git(top_directory).get_active_branch()
        assert name

        git = Git(local_repository=top_directory)
        task = _Task(name, top_directory, git)
        assert os.path.exists(_task_dir(task.top_directory, task.name))

        return task

    def __init__(self, name, top_directory, git):
        """
        Create a task with the given 'name' in the repository rooted in 'top_directory'.
        It is expected that the repository contains a git branch with same name as the task name and that there is a
        task description file in the pm/tasks directory, again, with the task name as file name.
        """
        assert isinstance(name, str)
        assert isinstance(top_directory, str)
        assert os.path.isdir(top_directory)
        self.name = name
        self.top_directory = top_directory
        self._git = git
        self.is_local = name in git.branches
        self.is_remote = name in git.origin_branches
        self.is_archived_remote = 'archive/' + name in git.origin_branches
        self.is_pm = os.path.exists(_task_dir(top_directory, name))

    def integrate(self, target_branch='development', archive_prefix='archive'):
        """
        Integrate this branch into the upstream branch 'target_branch' and archive it.
        A branch can only be successfully integrated after it has been reviewed and all reviewers have arrived at the
        'accepted' conclusion.
        """
        assert isinstance(target_branch, str)
        assert isinstance(archive_prefix, str)
        self._pre_integration_check()
        self._integrate(target_branch)
        self._archive(archive_prefix)

    def _pre_integration_check(self):
        """
        Check whether the current task is ready for integration.
        """
        try:
            self._check_is_active_branch()
            self._check_is_accepted()
        except _InvalidTaskStateError as e:
            raise _InvalidTaskStateError('Task {} is not ready for integration: {}'.format(self.name, e))

    def _check_is_active_branch(self):
        """
        Check whether this task is checked out as the active git branch in the task's repository.
        """
        active_branch = self._git.get_active_branch()
        if active_branch != self.name:
            raise _InvalidTaskStateError('Task {} is not the active checked-out branch ({}) in repository {}'.
                                         format(self.name, active_branch, self.top_directory))

    def _check_is_accepted(self):
        """
        Check whether all authors of completed reviews arrive at the 'accepted' conclusion in their final reviews, and
        that at least two review authors have done so.
        """
        done_reviews = self._get_concluded_reviews()
        if done_reviews == []:
            raise _InvalidTaskStateError('Task {} has not been reviewed'.format(self.name))
        for review in done_reviews:
            if not review.is_accepted():
                raise _InvalidTaskStateError('The conclusion of review {} for task {} is not "accepted"'.
                                             format(review.file_path, self.name))
        if len(done_reviews) < 2:
            raise _InvalidTaskStateError('Task {} does not have enough reviews (minimum: 2)'.format(self.name))

    def _get_concluded_reviews(self):
        """
        For any reviewer having reviewed this task, determine the most recent review and return all of them as a list
        of _Review instances.
        """
        reviews_by_author = {}
        for review in [r for r in self._get_reviews() if r.is_done()]:
            if review.author in reviews_by_author:
                if reviews_by_author[review.author].round < review.round:
                    reviews_by_author[review.author] = review
            else:
                reviews_by_author[review.author] = review
        return reviews_by_author.values()

    def _get_reviews(self):
        """
        Retrieve and return a list of _Review instances that represents all reviews of this task, even uncompleted
        ones.
        """
        reviews = []
        review_directory = _review_dir(self.top_directory, self.name)
        directory_contents = os.listdir(review_directory)
        directory_contents.sort()
        for relative_path in directory_contents:
            absolute_path = os.path.join(review_directory, relative_path)
            if os.path.isfile(absolute_path):
                review = _Review(absolute_path)
                reviews.append(review)
        return reviews

    def _integrate(self, target_branch):
        """
        Integrate this task by merging it into the given git 'target_branch' and marking it as complete.
        The resulting new state of 'target_branch' is pushed to the remote repository.
        """
        assert isinstance(target_branch, str)
        self._git.checkout(target_branch)
        self._git.merge_into_active_branch(self.name)
        self._complete()
        self._git.push(target_branch, target_branch)

    def _complete(self):
        """
        Mark this task as complete in the currently active git branch by moving the task description file into the
        'completed' sub-directory and committing the result.
        """
        task_dir = _task_dir(self.top_directory)
        src = os.path.join(task_dir, self.name)
        dst = os.path.join(task_dir, 'completed', self.name)
        self._git.move(src, dst)
        self._git.commit(msg='Mark task {} as completed'.format(self.name), files=[task_dir])

    def _archive(self, archive_prefix):
        """
        Archive this task by renaming it with the given 'archive_prefix' in both the local and the remote
        repositories.
        """
        assert isinstance(archive_prefix, str)
        archived_name = archive_prefix + '/' + self.name
        self._git.rename_branch(self.name, archived_name)
        self._git.push(archived_name, archived_name)
        self._git.delete_remote_branch(self.name)

    def related_branches(self):
        """Return a set of branch names that are related to this branch.

        Another branch is related if it contains any of the same
        commits as this task's branch that are not already on the
        development branch.

        """
        if self.is_local:
            ahead_list = self._git.ahead_list(self.name, 'origin/development')
            if len(ahead_list) > 100:  # Some branches are so far diverged we ignore them
                return set()
            return self._git.branch_contains(ahead_list) - {self.name}
        return set()

    def report_line(self):
        """Return a one line summary of the task for report printing purposes."""
        date = self.last_commit()
        if date is None:
            date_str = ''
        else:
            date_str = time.strftime('%Y-%m-%d %H:%M', time.localtime(date))

        if self.is_local and self.is_remote:
            vs_remote = self._git.ahead_behind_string(self.name, 'origin/' + self.name)
        else:
            vs_remote = ''

        if self.is_remote:
            vs_dev = self._git.ahead_behind_string('origin/' + self.name, 'origin/development')
        else:
            vs_dev = ''

        return '{}{}{}{} | {:20} | {:12} | {:12} | {}'.format(
            'D' if self.is_pm else ' ',
            'L' if self.is_local else ' ',
            'R' if self.is_remote else ' ',
            'A' if self.is_archived_remote else ' ',
            date_str,
            vs_remote,
            vs_dev,
            self.name)

    def last_commit(self):
        """Return the date (as a UNIX timestamp) of the last commit.

        The last commit on the local branch is preferred over the remote branch.
        If the task has no branches, then None is returned.

        """
        if self.is_local:
            return self._git.branch_date(self.name)
        elif self.is_remote:
            return self._git.branch_date('origin/' + self.name)
        else:
            return None


@subcmd(cmd="task")
def list(args):
    git = Git(local_repository=args.topdir)
    task_dir = _task_dir(args.topdir)
    skipped_branches = ['development', 'master']
    task_names = set.union({t for t in os.listdir(task_dir) if
                            os.path.isfile(os.path.join(task_dir, t))},
                           {t.split('/')[0] for t in git.branches + git.origin_branches if
                            t.count('/') == 0 and t not in skipped_branches})

    print("flags| last commit          | +- vs origin | +- vs devel  | name")
    for t in sorted(task_names):
        task = _Task(t, args.topdir, git)
        print(task.report_line())
        for rel in task.related_branches():
            if rel in skipped_branches:
                continue
            branch = 'origin/' + t
            if branch not in git.get_remote_branches():
                branch = t
            base_branch = 'origin/' + rel
            if base_branch not in git.get_remote_branches():
                base_branch = rel
            print("{}{} ({})".format(' ' * 60, rel, git.ahead_behind_string(branch, base_branch).strip()))

    for check_branch in skipped_branches:
        if check_branch in git.branches:
            dev_ab = git.ahead_behind_string(check_branch, 'origin/' + check_branch).strip()
            if dev_ab != '':
                print("Warning: {} branch not in sync with origin: {}".format(check_branch, dev_ab))

    print('')
    print("D: described in pm/tasks  L: local branch  R: remote branch  A: archived branch")


@subcmd(cmd="task", help='Integrate a completed development task branch into the main upstream branch.',
        args=(Arg('--repo', help='Path of git repository to operate in. Defaults to current working directory.'),
              Arg('--name', help='Name of the task branch to integrate. Defaults to active branch in repository.'),
              Arg('--target', help='Name of branch to integrate task branch into. Defaults to "development".',
                  default='development'),
              Arg('--archive', help='Prefix to add to task branch name when archiving it. Defaults to "archive".',
                  default='archive')))
def integrate(args):
    """
    Integrate a completed development task/branch into the main upstream branch.
    """
    task = _Task.create(args.name, args.repo)
    task.integrate(args.target, args.archive)
