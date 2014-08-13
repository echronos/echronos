from pylib.xunittest import teamcityskip
from pylib.utils import Git
from pylib.components import sort_typedefs
from pylib.tasks import _Review
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
        assert sort_typedefs('\n'.join(x)) == expected


def test_full_stop_in_reviewer_name():
    with tempfile.TemporaryDirectory() as dir:
        round = 0
        author = 'john.doe'
        review_file_path = os.path.join(dir, 'review-{}.{}'.format(round, author))
        open(review_file_path, 'w').close()
        review = _Review(review_file_path)
        assert review.author == author
        assert review.round == round
