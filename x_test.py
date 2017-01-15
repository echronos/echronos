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

from pylib.utils import BASE_DIR
from pylib.components import _sort_typedefs, _sort_by_dependencies, _DependencyNode, _UnresolvableDependencyError
from pylib.task import _Review, Task, _InvalidTaskStateError, TaskConfiguration
from pylib.task_commands import task_cfg
from nose.tools import assert_raises
import itertools
import os
import tempfile
import unittest


def test_empty():
    """Test whether an empty test can be run at all given the test setup in x.py."""
    pass


def test_sort_typedefs():
    typedefs = ['typedef uint8_t foo;',
                'typedef foo bar;',
                'typedef bar baz;']
    expected = '\n'.join(typedefs)
    for x in itertools.permutations(typedefs):
        assert _sort_typedefs('\n'.join(x)) == expected


def test_full_stop_in_reviewer_name():
    with tempfile.TemporaryDirectory() as temp_dir:
        round_ = 0
        author = 'john.doe'
        review_file_path = os.path.join(temp_dir, '{}.{}.md'.format(author, round_))
        open(review_file_path, 'w').close()
        review = _Review(review_file_path)
        assert review.author == author
        assert review.round == round_


def test_resolve_dependencies():
    N = _DependencyNode
    a = N(('a',), ('b', 'c'))
    b = N(('b',), ('c',))
    c = N(('c',), ())
    nodes = (a, b, c)
    output = list(_sort_by_dependencies(nodes))
    assert output == [c, b, a]


def test_resolve_unresolvable_dependencies():
    N = _DependencyNode
    a = N(('a',), ('b',))
    b = N(('b',), ('c',))
    nodes = (a, b)
    try:
        _ = list(_sort_by_dependencies(nodes))
        assert False
    except _UnresolvableDependencyError:
        pass


def test_resolve_cyclic_dependencies():
    N = _DependencyNode
    a = N(('a',), ())
    b = N(('b',), ('a',))
    c = N(('c',), ('b', 'd'))
    d = N(('d',), ('c',))
    nodes = (a, b, c, d)
    try:
        output = list(_sort_by_dependencies(nodes))
        assert False
    except _UnresolvableDependencyError:
        pass
    output = list(_sort_by_dependencies(nodes, ignore_cyclic_dependencies=True))
    assert sorted(output) == sorted(nodes)


# Workaround for the following tests for the pre-integration check that don't use the Git module
class DummyGit:
    def __init__(self, task_name):
        self.branches = []
        self.remote_branches = frozenset(["archive/%s" % task_name])


# Helper for the pre-integration check tests
def task_dummy_create(task_name):
    cfg = TaskConfiguration(repo_path=BASE_DIR,
                            tasks_path=os.path.join('x_test_data', 'tasks'),
                            description_template_path=task_cfg.description_template_path,
                            reviews_path=os.path.join('x_test_data', 'reviews'),
                            mainline_branch=task_cfg.mainline_branch)
    return Task(cfg, task_name, checkout=False)


@unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
def test_task_accepted():
    # This task was accepted without any rework reviews
    task_dummy_create("test_task_accepted")._check_is_accepted()


@unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
def test_rework_is_accepted():
    # This task had a rework review that was later accepted by its review author
    task_dummy_create("test_rework_is_accepted")._check_is_accepted()


@unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
def test_rework_not_accepted():
    # This task was erroneously integrated with a rework review not later accepted by its review author
    assert_raises(_InvalidTaskStateError, task_dummy_create("test_rework_not_accepted")._check_is_accepted)


@unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
def test_not_enough_accepted():
    # This task was integrated after being accepted by only one reviewer, before we placed a hard minimum in the check
    assert_raises(_InvalidTaskStateError, task_dummy_create("test_not_enough_accepted")._check_is_accepted)
