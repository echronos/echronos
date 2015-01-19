from pylib.xunittest import teamcityskip
from pylib.utils import Git
from pylib.components import _sort_typedefs, _sort_by_dependencies, _DependencyNode, _UnresolvableDependencyError
from pylib.tasks import _Review, _Task, _InvalidTaskStateError
import itertools
import os
import tempfile

# The constants refer to the base (initial) commit.
# All branches should be dervied from this commit, so it
# should always be available. This commit is used to test
# some of the 'Git' class functionality.
INITIAL_COMMIT = '052c07259121ae27a0736dfe92cd5b072ecc5745'
INITIAL_TIME = 1364960632


def test_empty():
    """Test whether an empty test can be run at all given the test setup in x.py."""
    pass


@teamcityskip
def test_git_branch_hash():
    g = Git()
    assert INITIAL_COMMIT == g.branch_hash(INITIAL_COMMIT)


@teamcityskip
def test_git_branch_date():
    g = Git()
    assert INITIAL_TIME == g.branch_date(INITIAL_COMMIT)


def test_sort_typedefs():
    typedefs = ['typedef uint8_t foo;',
                'typedef foo bar;',
                'typedef bar baz;']
    expected = '\n'.join(typedefs)
    for x in itertools.permutations(typedefs):
        assert _sort_typedefs('\n'.join(x)) == expected


def test_full_stop_in_reviewer_name():
    with tempfile.TemporaryDirectory() as dir:
        round = 0
        author = 'john.doe'
        review_file_path = os.path.join(dir, 'review-{}.{}'.format(round, author))
        open(review_file_path, 'w').close()
        review = _Review(review_file_path)
        assert review.author == author
        assert review.round == round


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
        output = list(_sort_by_dependencies(nodes))
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
        self.origin_branches = ["archive/%s" % task_name]


# Return a boolean indicating whether the x.py pre-integration check would consider the task "accepted" by its reviews
def task_check_is_accepted(task_name):
    task = _Task(task_name, os.getcwd(), DummyGit(task_name))
    accepted = False
    try:
        task._check_is_accepted()
        accepted = True
    except _InvalidTaskStateError as e:
        pass
    return accepted


def test_task_accepted():
    # This task was accepted without any rework reviews
    assert task_check_is_accepted("eeZMmO-cpp-friendly-headers")


def test_rework_is_accepted():
    # This task had a rework review that was later accepted by its review author
    assert task_check_is_accepted("ogb1UE-kochab-documentation-base")


def test_rework_not_accepted():
    # This task was erroneously integrated with a rework review not later accepted by its review author
    assert not task_check_is_accepted("g256JD-kochab-mutex-timeout")


def test_not_enough_accepted():
    # This task was integrated after being accepted by only one reviewer, before we placed a hard minimum in the check
    assert not task_check_is_accepted("65N0RS-fix-x-test-regression")
