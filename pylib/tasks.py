import os
import re
import time
import string
import datetime
import subprocess
from random import choice


def gen_tag():
    tag_length = 6
    tag_chars = string.ascii_letters + string.digits
    return ''.join(choice(tag_chars) for _ in range(tag_length))


review_template = """Breakaway Task Review
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


def new_review(args):
    """Create a new review for the current branch."""
    # Check the directory is clean
    status = subprocess.check_output(['git', 'status', '--porcelain'], cwd=args.topdir)
    if status != b'':
        print("Can't commit while directory is dirty. Aborting.")
        return 1

    branch = subprocess.check_output(['git', 'symbolic-ref', 'HEAD'], cwd=args.topdir).decode().strip().split('/')[-1]
    review_dir = os.path.join(args.topdir, 'pm', 'reviews', branch)

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
            f.write(review_template % params)

    git = Git(local_repository=args.topdir)
    # now, git add, git commit -m <msg> and git push.
    msg = 'Review request {} for {}'.format(review_number, branch)
    git.add(review_files)
    git.commit(msg)
    git.push(branch, branch)


task_template = """Task: {}
==============================================================================

Motivation
----------


Goals
--------


Test Plan
---------

"""


def new_task(args):
    """Create a new task."""
    remote = 'origin'
    branch_from = remote + '/development'
    tasks_dir = os.path.join(args.topdir, 'pm', 'tasks')

    git = Git(local_repository=args.topdir)
    if not git.working_dir_clean():
        print("Working directory must be clean before creating a new task.")
        return 1

    if args.fetch:
        # Ensure that we have the very last origin/development to branch
        # from.
        git.fetch()

    fullname = gen_tag() + '-' + args.taskname
    git.branch(fullname, branch_from, track=False)
    git.push(fullname, fullname, set_upstream=True)
    git.checkout(fullname)

    task_fn = os.path.join(tasks_dir, fullname)
    with open(task_fn, 'w', newline='\n') as f:
        f.write(task_template.format(fullname))

    print("Edit file: {} then add/commit/push.".format(task_fn))
    print('Suggest committing as: git commit -m "New task: {}"'.format(fullname))


class Git:
    """
    Represents common state applicable to a series of git invocations and provides a pythonic interface to git.
    """
    def __init__(self, local_repository=os.getcwd(), remote_repository='origin'):
        """
        Create a Git instance with which all commands operate with the given local and remote repositories.
        """
        assert isinstance(local_repository, str)
        assert isinstance(remote_repository, str)
        # Check whether the local repository is a directory managed by git.
        # Note that '.git' can be a directory in case of a git repository or a file in case of a git submodule
        assert os.path.exists(os.path.join(local_repository, '.git'))
        self.local_repository = local_repository
        self.remote_repository = remote_repository
        self._sep = None
        self._branches = None
        self._remote_branches = None

    def convert_path_separators(self, files):
        """
        If necessary, convert the path separators in the path or paths given in 'files' to the path separator expected
        by the command-line git tool.
        These separators differ when a cygwin git command-line tool is used with a native Windows python installation.
        """
        assert isinstance(files, (str, list))
        if isinstance(files, str):
            return files.replace(os.sep, self.sep)
        else:
            return [file.replace(os.sep, self.sep) for file in files]

    @property
    def sep(self):
        """
        Return the (potentially cached) path separator expected by the git command-line tool.
        """
        if self._sep is None:
            self._sep = self._get_sep()
        return self._sep

    def _get_sep(self):
        """
        Determine the path separator expected by the git command-line tool.
        """
        output = self._do(['ls-tree', '-r', '--name-only', 'HEAD:pm/tasks'])
        for line in output.splitlines():
            if line.startswith('completed'):
                line = line.replace('completed', '', 1)
                return line[0]
        raise LookupError('git ls-tree does not list any files in pm/tasks/completed as expected')

    @property
    def branches(self):
        """List of local branches."""
        if self._branches is None:
            self._branches = self._get_branches()
        return self._branches

    def _get_branches(self):
        """Return a list of local branches."""
        return [x[2:] for x in self._do(['branch'], as_lines=True)]

    @property
    def origin_branches(self):
        """List of origin-remote branches"""
        return self.get_remote_branches()

    def get_remote_branches(self, remote='origin'):
        """Return a list of remote branches. Remote defaults to 'origin'."""
        if self._remote_branches is None:
            self._remote_branches = [x[2:].strip() for x in self._do(['branch', '-r'], as_lines=True)]
        return [x[len(remote) + 1:] for x in self._remote_branches if x.startswith(remote + '/')]

    def _do(self, parameters, as_lines=False):
        """
        Execute the git command line tool with the given command-line parameters and return the console output as a
        string.
        """
        assert type(parameters) == list
        raw_data = subprocess.check_output(['git'] + parameters, cwd=self.local_repository).decode()
        if as_lines:
            return raw_data.splitlines()
        else:
            return raw_data

    def get_active_branch(self):
        """
        Determine the currently active branch in the local git repository and return its name as a string.
        """
        pattern = '* '
        for line in self._do(['branch'], as_lines=True):
            if line.startswith(pattern):
                return line.split(' ', maxsplit=1)[1].strip()
        raise LookupError('No active branch in git repository ' + self.local_repository)

    def branch(self, name, start_point=None, *, track=None):
        """Create a new branch, optionally from a specific start point.

        If track is set to True, then '--track' will be passed to git.
        If track is set to False, then '--no-track' will be passed to git.
        If track is None, then no tracking flag will be passed to git.

        """
        params = ['branch']
        if not track is None:
            params.append('--track' if track else '--no-track')
        params.append(name)
        if not start_point is None:
            params.append(start_point)
        return self._do(params)

    def set_upstream(self, upstream, branch=None):
        """Set the upstream / tracking branch of a given branch.

        If branch is None, it defaults to the current branch.

        """
        params = ['branch', '-u', upstream]
        if branch:
            params.append(branch)

        return self._do(params)

    def checkout(self, revid):
        """
        Check out the specified revision ID (typically a branch name) in the local repository.
        """
        assert isinstance(revid, str)
        return self._do(['checkout', revid])

    def merge_into_active_branch(self, revid):
        """
        Merge the specified revision ID into the currently active branch.
        """
        assert isinstance(revid, str)
        return self._do(['merge', revid])

    def fetch(self):
        """Fetch from the remote origin."""
        return self._do(['fetch'])

    def push(self, src=None, dst=None, *, force=False, set_upstream=False):
        """Push the local revision 'src' into the remote branch 'dst', optionally forcing the update.

        If 'set_upstream' evaluates to True, 'dst' is set as the upstream / tracking branch of 'src'.

        """
        assert src is None or isinstance(src, str)
        assert dst is None or isinstance(dst, str)
        assert isinstance(force, bool)
        revspec = ''
        if src:
            revspec = src
        if dst:
            revspec += ':' + dst
        if revspec == '':
            revspec_args = []
        else:
            revspec_args = [revspec]
        if force:
            force_option = ['--force']
        else:
            force_option = []
        if set_upstream:
            set_upstream_option = ['-u']
        else:
            set_upstream_option = []
        return self._do(['push'] + force_option + set_upstream_option + [self.remote_repository] + revspec_args)

    def move(self, src, dst):
        """
        Rename a local resource from its old name 'src' to its new name 'dst' or move a list of local files 'src' into
        a directory 'dst'.
        """
        assert isinstance(src, (str, list))
        assert isinstance(dst, str)
        if type(src) == str:
            src_list = [src]
        else:
            src_list = src
        return self._do(['mv'] + self.convert_path_separators(src_list) + [self.convert_path_separators(dst)])

    def add(self, files):
        """Add the list of files to the index in preparation of a future commit."""
        return self._do(['add'] + self.convert_path_separators(files))

    def commit(self, msg, files=None):
        """Commit the changes in the specified 'files' with the given 'message' to the currently active branch.

        If 'files' is None (or unspecified), all staged files are committed.

        """
        assert isinstance(msg, str)
        assert files is None or isinstance(files, list)
        if files is None:
            file_args = []
        else:
            file_args = self.convert_path_separators(files)
        return self._do(['commit', '-m', msg] + file_args)

    def rename_branch(self, src, dst):
        """
        Rename a local branch from its current name 'src' to the new name 'dst'.
        """
        assert isinstance(src, str)
        assert isinstance(dst, str)
        return self._do(['branch', '-m', src, dst])

    def delete_remote_branch(self, branch):
        assert isinstance(branch, str)
        return self.push(dst=branch)

    def ahead_list(self, branch, base_branch):
        """Return a list of SHAs for the commits that are in branch, but not base_branch"""
        return self._do(['log', '{}..{}'.format(base_branch, branch), '--pretty=format:%H'], as_lines=True)

    def branch_contains(self, commits):
        """Return a set of branches that contain any of the commits."""
        contains = set()
        for c in commits:
            for b in self._do(['branch', '--contains', c], as_lines=True):
                contains.add(b[2:])
        return contains

    def count_commits(self, since, until):
        """Return the number of commit between two commits 'since' and 'until'.

        See git log --help for more details.

        """
        return len(self._do(['log', '{}..{}'.format(since, until), '--pretty=oneline'], as_lines=True))

    def ahead_behind(self, branch, base_branch):
        """Return the a tuple for how many commits ahead/behind a branch is when compared
        to a base_branch.

        """
        return self.count_commits(base_branch, branch), self.count_commits(branch, base_branch)

    def ahead_behind_string(self, branch, base_branch):
        """Format git_ahead_behind() as a string for presentation to the user.

        """
        ahead, behind = self.ahead_behind(branch, base_branch)
        r = ''
        if behind > 0:
            r = '-{:<4} '.format(behind)
        else:
            r = '      '
        if ahead > 0:
            r += '+{:<4}'.format(ahead)
        return r

    def _log_pretty(self, pretty_fmt, branch=None):
        """Return information from the latest commit with a specified `pretty` format.

        The log from a specified branch may be specified.
        See `git log` man page for possible pretty formats.

        """
        # Future directions: Rather than just the latest commit, allow the caller
        # specify the number of commits. This requires additional parsing of the
        # result to return a list, rather than just a single item.
        # Additionally, the caller could pass a 'conversion' function which would
        # convert the string into a a more useful data-type.
        # As this method may be changed in the future, it is marked as a private
        # function (for now).
        cmd = ['log']
        if branch is not None:
            cmd.append(branch)
        cmd.append('-1')
        cmd.append('--pretty=format:{}'.format(pretty_fmt))
        return self._do(cmd).strip()

    def branch_date(self, branch=None):
        """Return the date of the latest commit on a given branch as a UNIX timestamp.

        The branch may be ommitted, in which case it defaults to the current head.

        """
        return int(self._log_pretty('%at', branch=branch))

    def branch_hash(self, branch=None):
        """Return the hash of the latest commit on a given branch as a UNIX timestamp.

        The branch may be ommitted, in which case it defaults to the current head.

        """
        return self._log_pretty('%H', branch=branch)

    def working_dir_clean(self):
        """Return True is the working directory is clean."""
        return self._do(['status', '--porcelain']) == ''


class Review:
    """
    Represents a review on a development task/branch.
    """
    def __init__(self, file_path):
        """
        Create a Review instance representing the review in the given 'file_path'.
        This provides easy access to the review's review round, author, and conclusion.
        """
        assert isinstance(file_path, str)
        assert os.path.isfile(file_path)
        self.file_path = file_path
        trunk, self.author = os.path.splitext(file_path)
        self.author = self.author[1:]
        relative_trunk = os.path.basename(trunk)
        self.round = int(relative_trunk.split('-')[-1])
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


class InvalidTaskStateError(RuntimeError):
    """
    To be raised when a task state transition cannot be performed because the task is not in the appropriate source
    state.
    """
    pass


class Task:
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
        Create and return a new Task instance, falling back to defaults if the optional task name and repository
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
        task = Task(name, top_directory, git)
        assert os.path.exists(task.get_description_file_name())

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
        self._reviews = None
        self._git = git
        self.is_local = name in git.branches
        self.is_remote = name in git.origin_branches
        self.is_archived_remote = 'archive/' + name in git.origin_branches
        self.is_pm = os.path.exists(os.path.join(top_directory, 'pm', 'tasks', name))

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
        except InvalidTaskStateError as e:
            raise InvalidTaskStateError('Task {} is not ready for integration: {}'.format(self.name, e))

    def _check_is_active_branch(self):
        """
        Check whether this task is checked out as the active git branch in the task's repository.
        """
        active_branch = self._git.get_active_branch()
        if active_branch != self.name:
            raise InvalidTaskStateError('Task {} is not the active checked-out branch ({}) in repository {}'.
                                        format(self.name, active_branch, self.top_directory))

    def _check_is_accepted(self):
        """
        Check whether all authors of completed reviews arrive at the 'accepted' conclusion in their final reviews.
        """
        all_reviews = self._get_most_recent_reviews_from_all_authors()
        done_reviews = filter(Review.is_done, all_reviews)
        if len(list(done_reviews)) == 0:
            raise InvalidTaskStateError('Task {} has not been reviewed'.format(self.name))
        for review in done_reviews:
            if not review.is_accepted():
                raise InvalidTaskStateError('The conclusion of review {} for task {} is not "accepted"'.
                                            format(review.file_path, self.name))

    def _get_most_recent_reviews_from_all_authors(self):
        """
        For any reviewer having reviewed this task, determine the most recent review and return all of them as a list
        of Review instances.
        """
        reviews_by_author = {}
        for review in self.reviews:
            if review.author in reviews_by_author:
                if reviews_by_author[review.author].round < review.round:
                    reviews_by_author[review.author] = review
            else:
                reviews_by_author[review.author] = review
        return reviews_by_author.values()

    @property
    def reviews(self):
        """
        Return a list of Review instances that represents all reviews of this task, even uncompleted ones.
        """
        if not self._reviews:
            self._reviews = self._get_reviews()
        return self._reviews

    def _get_reviews(self):
        """
        Retrieve and return a list of Review instances that represents all reviews of this task, even uncompleted
        ones.
        """
        reviews = []
        review_directory = os.path.join(self.top_directory, 'pm', 'reviews', self.name)
        directory_contents = os.listdir(review_directory)
        directory_contents.sort()
        for relative_path in directory_contents:
            absolute_path = os.path.join(review_directory, relative_path)
            if os.path.isfile(absolute_path):
                review = Review(absolute_path)
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
        Mark this taks as complete in the currently active git branch by moving the task description file into the
        'completed' sub-directory and committing the result.
        """
        src = os.path.join('pm', 'tasks', self.name)
        dst = os.path.join('pm', 'tasks', 'completed', self.name)
        self._git.move(src, dst)
        self._git.commit(msg='Mark task {} as completed'.format(self.name), files=[os.path.join('pm', 'tasks')])

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

    @property
    def description(self):
        """
        Return the (potentially cached) description of this task as a string.
        """
        if self._description is None:
            self._description = self._get_description()
        return self._description

    def _get_description(self):
        """
        Retrieve the description of this task from the file system and return it as a string.
        """
        return open(self.get_description_file_name()).read()

    def get_description_file_name(self):
        """
        Return the name of the description file of this task as a string.
        """
        return os.path.join(self.top_directory, 'pm', 'tasks', self.name)

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


def tasks(args):
    git = Git(local_repository=args.topdir)
    task_dir = os.path.join(args.topdir, 'pm', 'tasks')
    skipped_branches = ['development', 'master']
    task_names = set.union({t for t in os.listdir(task_dir) if
                            os.path.isfile(os.path.join(task_dir, t))},
                           {t.split('/')[0] for t in git.branches + git.origin_branches if
                            t.count('/') == 0 and t not in skipped_branches})

    print("flags| last commit          | +- vs origin | +- vs devel  | name")
    for t in sorted(task_names):
        task = Task(t, args.topdir, git)
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


def integrate(command_line_options):
    """
    Integrate a completed development task/branch into the main upstream branch.
    """
    task = Task.create(command_line_options.name, command_line_options.repo)
    task.integrate(command_line_options.target, command_line_options.archive)
