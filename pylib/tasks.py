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
import string
import shutil
import subprocess
from random import choice
from .utils import Git, find_path, string_to_path
from .cmdline import subcmd, Arg

_offline_arg = Arg('-o', '--offline', action='store_true',
                   help='Skip all git commands that require an Internet connection')
_taskname_arg = Arg('taskname', nargs='?', help='The name of the task to manage. Defaults to the active git branch.')

_LOCAL_MAINLINE='development'
_REMOTE_MAINLINE='origin/' + _LOCAL_MAINLINE


@subcmd(cmd="task", help='Generate a random 6-char alphanumeric string')
def tag(_):
    tag_length = 6
    tag_chars = string.ascii_letters + string.digits
    return ''.join(choice(tag_chars) for _ in range(tag_length))


@subcmd(cmd="task",
        args=(_offline_arg, Arg('taskname', help='The name of the task to manage.')),
        help='Developers: create a new task to work on, including a template for the task description.')
def create(args):
    if not _Task._is_valid_name(args.taskname):
        print('The task name "{}"" contains unsupported characters. '
              'Only letters, digits, dashes, and underscores are supported.'.format(args.taskname))
        return 1

    git = Git(local_repository=args.topdir)
    if not git.is_clean():
        print('The local git repository contains staged or unstaged changes.')
        return 1

    if not git.is_ref_uptodate_with_tracking_branch(offline=args.offline):
        print('The active branch is not up-to-date with its remote tracking branch.')
        return 1

    fullname = tag(None) + '-' + args.taskname
    if fullname in git.branches:
        print('The task name "{}" is not unique as the git branch "{}" already exists.'
              .format(fullname, fullname))
        return 1

    archived_name = _Task.ARCHIVE_PREFIX + '/' + fullname
    if archived_name in git.remote_branches:
        print('The task name "{}" is not unique as the archived git branch "{}" already exists.'
              .format(fullname, archived_name))
        return 1

    git.branch(fullname, _REMOTE_MAINLINE, track=False)
    git.checkout(fullname)
    if not offline:
        git.push(fullname, set_upstream=True)

    template_path = find_path('.github/PULL_REQUEST_TEMPLATE.md', args.topdir)
    task_fn = _task_dir(args.topdir, fullname)
    shutil.copyfile(template_path, task_fn)
    git.add(task_fn)

    print('1. edit file "{}"\n'
          '2. commit via #> git commit -a -m "New task: {}"\n'
          '3. push task to remote repository via #> git push'.format(task_fn, fullname))


@subcmd(cmd="task",
        args=(_offline_arg, _taskname_arg),
        help='Developers: bring a task up-to-date with the latest changes on the mainline branch "{}". '
             'If the task is not yet on review, this rebases the task branch onto the mainline branch. '
             'If the task is on review, the mainline changes are merged into the task branch.'
             .format(_REMOTE_MAINLINE))
def update(args):
    task = _Task.instantiate(name=args.taskname)
    return task.update(offline=args.offline)


@subcmd(cmd="task",
        args=(_offline_arg, _taskname_arg),
        help='Developers: request reviews for a task.')
def request_reviews(args):
    """Request reviews for a task branch by mark it as up for review."""
    task = _Task.instantiate(name=args.taskname)
    return task.request_reviews(args.offline)


@subcmd(cmd="task",
        args=(_offline_arg, _taskname_arg,
              Arg('-a', '--accept', action='store_true',
                  help='Create and complete the review with the conclusion "accepted", commit, and push it. '
                       'This is an alias for the command "x.py task accept".')),
        help='Reviewers: create a stub for a new review of the active task branch.')
def review(args):
    task = _Task.instantiate(name=args.taskname)
    return task.review(offline=args.offline, accept=args.accept)


@subcmd(cmd="task",
        args=(_offline_arg, _taskname_arg),
        help='Reviewers: create, commit, and push a new review for the active task branch with the conclusion '
             '"Accepted". This is an alias for the command "x.py task review --accept".')
def accept(args):
    args.accept = True
    return review(args)


@subcmd(cmd="task", help='Developers: integrate a completed task branch into the mainline branch.',
        args=(_taskname_arg,
              Arg('--target', help='Name of branch to integrate task branch into. Defaults to "{}".'.
                  format(_LOCAL_MAINLINE), default=_LOCAL_MAINLINE)))
def integrate(args):
    """
    Integrate a completed development task/branch into the mainline branch.
    """
    task = _Task.instantiate(name=args.taskname)
    task.integrate(args.target)


class _TaskNotActiveBranchError(RuntimeError):
    """
    To be raised when a Task object does not represent the currently active branch in the local git repository.
    """
    pass


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

    ARCHIVE_PREFIX = 'archive'

    @staticmethod
    def instantiate(name=None, checkout=True):
        """
        Create and return a new _Task instance, falling back to defaults if the optional task name and repository
        directory are not specified.
        If 'name' is not specified, it defaults to the name of the active branch in the task's top directory.
        If 'checkout' is true, this function checks out the git branch 'name' in the local git repository.
        If 'checkout' is false, this function does not modify the active git branch in the local git repository.
        """
        git = Git()

        if name is None:
            name = git.get_active_branch()
        else:
            if checkout:
                git.checkout(name)

        return _Task(name, os.getcwd(), git)

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
        self._review_dir = _review_dir(self.top_directory, self.name)
        self._review_placeholder_path = os.path.join(self._review_dir,
                                                     '.placeholder_for_git_to_not_remove_this_otherwise_empty_dir')

    def integrate(self, target_branch=_LOCAL_MAINLINE, archive_prefix=ARCHIVE_PREFIX):
        """
        Integrate this branch into the mainline branch 'target_branch' and archive it.
        A branch can only be successfully integrated after it has been reviewed and all reviewers have arrived at the
        'accepted' conclusion.
        """
        assert isinstance(target_branch, str)
        assert isinstance(archive_prefix, str)
        self._pre_integration_check()
        self._integrate(target_branch)
        self._archive(archive_prefix)

    def _pre_integration_check(self):
        self._check_and_prepare(offline=False)
        self._check_is_accepted()

    def _check_is_accepted(self):
        """
        Check whether all authors of completed reviews arrive at the 'accepted' conclusion in their final reviews, and
        that at least two review authors have done so.
        """
        reviews = self._get_most_recent_reviews()
        if not reviews:
            raise _InvalidTaskStateError('Task {} has not been reviewed'.format(self.name))
        for review in reviews:
            if not review.is_accepted():
                raise _InvalidTaskStateError('The conclusion of review {} for task {} is not "accepted"'.
                                             format(review.file_path, self.name))
        if len(reviews) < 2:
            raise _InvalidTaskStateError('Task {} does not have enough reviews (minimum: 2)'.format(self.name))

    def _get_most_recent_reviews(self):
        """
        For any reviewer having reviewed this task, determine the most recent review and return all of them as a list
        of _Review instances.
        """
        reviews_by_author = {}
        for review in self._get_reviews():
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
        directory_contents = os.listdir(self._review_dir)
        directory_contents.sort()
        for relative_path in directory_contents:
            absolute_path = os.path.join(self._review_dir, relative_path)
            if os.path.isfile(absolute_path) and not relative_path.startswith('.'):
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
        self._git.push(target_branch)

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
        self._git.push(archived_name)
        self._git.delete_remote_branch(self.name)

    def request_reviews(self, offline=False):
        """Mark the current branch as up for review.

        - create the directory pm/reviews/<task_name>
        - create the empty file pm/reviews/<task_name>/.placeholder_for_git_to_not_remove_this_otherwise_empty_dir
        - commit the result
        - push the commit to the remote

        """
        self._check_and_prepare(offline=offline)
        if self._is_on_review():
            raise _InvalidTaskStateError('The task {} is already on review.'.format(self.name))

        os.makedirs(self._review_dir, exist_ok=False)
        open(self._review_placeholder_path, 'w').close()

        self._git.add(files=[self._review_dir])
        self._git.commit('Review request for {}'.format(self.name), files=[self._review_dir])

        if not offline:
            self._git.push()

        return 0

    def review(self, offline=False, accept=False):
        self._check_and_prepare(offline=offline)
        if not self._is_on_review():
            raise _InvalidTaskStateError('The task {} is not on review.'.format(self.name))

        reviewer = self._git.get_user_name()
        review_path_template = os.path.join(self._review_dir, '{}.{{}}.md'.format(string_to_path(reviewer)))
        for review_round in range(1000):
            review_path = review_path_template.format(review_round)
            if not os.path.exists(review_path):
                break
        else:
            raise FileNotFoundError('Unable to determine review round for task "{}" and reviewer "{}" ("{}")'.format(branch, reviewer, review_path_template))

        if not accept:
            review_template = """Reviewer: {} ({})
Conclusion: Accepted/Rework

Location: filename:linenum
Comment:
"""
        else:
            review_template = """Reviewer: {} ({})
Conclusion: Accepted
"""
        review_contents = review_template.format(reviewer, self._git.get_user_email())
        open(review_path, 'wb').write(review_contents.encode('UTF-8'))
        self._git.add([review_path])
        if os.path.exists(self._review_placeholder_path):
            self._git.rm([self._review_placeholder_path])

        if not accept:
            print('To complete the review, edit the file "{0}" and commit and push it with the commands\n\
    git commit -a\n\
    git push'.format(review_path))
        else:
            self._git.commit('Review task {}: accepted, 0 comments'.format(self.name))
            if not offline:
                self._git.push()

        return 0

    def update(self, offline=False):
        self._check_and_prepare(offline=offline, check_mainline=False)

        if self._is_on_review():
            self._git.merge_into_active_branch(_REMOTE_MAINLINE, '--no-squash')
        else:
            self._git.rebase(_REMOTE_MAINLINE)

        if not offline:
            self._git.push(force=not self._is_on_review())

        return 0

    def _check_and_prepare(self, check_mainline=True, offline=False):
        active_branch = self._git.get_active_branch()
        if active_branch != self.name:
            raise _InvalidTaskStateError('Task {} is not the active checked-out branch ({}) in repository {}'.
                                         format(self.name, active_branch, self.top_directory))

        if not self._git.is_clean():
            raise _InvalidTaskStateError('The local git repository contains staged or unstaged changes.')

        if not self._git.is_ref_uptodate_with_tracking_branch(offline=offline):
            try:
                self._git.merge_into_active_branch(self._git.get_tracking_branch(), '--ff-only')
            except subprocess.CalledProcessError:
                raise _InvalidTaskStateError('The task branch is not up-to-date with its remote tracking branch.')

        if check_mainline and not self.name in self._git.get_branches_that_contain_revid(_REMOTE_MAINLINE):
            self.update(offline=offline)

    def _is_on_review(self):
        if self._git.get_active_branch() != self.name:
            raise _TaskNotActiveBranchError('The task {} is not the active git branch (the active git branch is {})'.format(self.name, self._git.get_active_branch()))
        return os.path.exists(self._review_dir)

    @staticmethod
    def _is_valid_name(name):
        return all([c.isalnum() or c in ('-', '_') for c in name])


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
        author_and_round = os.path.splitext(basename)[0].split('-', maxsplit=1)[1]
        author, round = author_and_round.split('.', maxsplit=1)
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
        return self.conclusion in ('accept', 'accepted')

    def is_rework(self):
        return self.conclusion == 'rework'


def _task_dir(topdir, *args):
    return os.path.join(topdir, 'pm', 'tasks', *args)


def _review_dir(topdir, *args):
    return os.path.join(topdir, 'pm', 'reviews', *args)
