from util.util import do_nothing, Singleton, s16l, check_unique


def test_do_nothing_no_args():
    do_nothing()


def test_do_nothing_some_args():
    do_nothing(1, 2, 3)


def test_do_nothing_kw_args():
    do_nothing(x=1, b=2, c=3)


def test_singleton():
    x = Singleton('x')
    x1 = x
    y = Singleton('y')
    y1 = y

    assert x1 is x
    assert x1 == x
    assert y1 is y
    assert y1 == y
    assert x is not y

    assert str(x) == '<Singleton: x>'
    assert str(y) == '<Singleton: y>'


def check_s16l(value, n, expected):
    assert s16l(value, n) == expected


def test_s16l_zero():
    for n in range(16):
        yield 'n={}'.format(n), check_s16l, 0, n, 0


def test_s16l_ffff():
    for n, expected in [(1, 0xfffe), (8, 0xff00), (15, 0x8000), (16, 0)]:
        yield 'n={}'.format(n), check_s16l, 0xffff, n, expected
