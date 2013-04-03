import util.util


def test_do_nothing_no_args():
    util.util.do_nothing()


def test_do_nothing_some_args():
    util.util.do_nothing(1, 2, 3)


def test_do_nothing_kw_args():
    util.util.do_nothing(x=1, b=2, c=3)


def test_singleton():
    x = util.util.Singleton('x')
    x1 = x
    y = util.util.Singleton('y')
    y1 = y

    assert x1 is x
    assert x1 == x
    assert y1 is y
    assert y1 == y
    assert x is not y

    assert str(x) == '<Singleton: x>'
    assert str(y) == '<Singleton: y>'


def check_s16l(value, n, expected):
    assert util.util.s16l(value, n) == expected


def test_s16l_zero():
    for n in range(16):
        yield 'n={}'.format(n), check_s16l, 0, n, 0


def test_s16l_ffff():
    for n, expected in [(1, 0xfffe), (8, 0xff00), (15, 0x8000), (16, 0)]:
        yield 'n={}'.format(n), check_s16l, 0xffff, n, expected
