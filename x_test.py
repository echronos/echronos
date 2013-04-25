from x import *

# The constants refer to the base (initial) commit.
# All branches should be dervied from this commit, so it
# should always be available. This commit is used to test
# some of the 'Git' class functionality.
INITIAL_COMMIT = '052c07259121ae27a0736dfe92cd5b072ecc5745'
INITIAL_TIME = 1364960632


def test_empty():
    """Test whether an empty test can be run at all given the test setup in x.py."""
    pass


def test_git_branch_hash():
    g = Git()
    assert INITIAL_COMMIT == g.branch_hash(INITIAL_COMMIT)


def test_git_branch_date():
    g = Git()
    assert INITIAL_TIME == g.branch_date(INITIAL_COMMIT)
