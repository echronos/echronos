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

import unittest
from util.util import do_nothing, Singleton, s16l, check_unique, remove_multi, add_index, \
    LengthMixin, LengthList, config_traverse, config_set, list_search


class TestCase(unittest.TestCase):
    def test_do_nothing_no_args(self):
        do_nothing()

    def test_do_nothing_some_args(self):
        do_nothing(1, 2, 3)

    def test_do_nothing_kw_args(self):
        do_nothing(x=1, b=2, c=3)

    def test_singleton(self):
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

    def test_s16l_zero(self):
        for n in range(16):
            yield 'n={}'.format(n), check_s16l, 0, n, 0

    def test_s16l_ffff(self):
        for n, expected in [(1, 0xfffe), (8, 0xff00), (15, 0x8000), (16, 0)]:
            yield 'n={}'.format(n), check_s16l, 0xffff, n, expected

    def test_check_unique_no_dups(self):
        check_unique(range(40))

    def test_check_unique_dups(self):
        lst = list(range(40)) + [39]
        with self.assertRaises(ValueError) as cm:
            check_unique(lst)
        assert str(cm.exception) == "Duplicates found in list: [(39, 2)]"

    def test_remove_multi(self):
        x = list(range(3))
        remove_multi(x)
        assert x == list(range(3))

        remove_multi(x, 0, 2)
        assert x == [1]

        x = list(range(3))
        remove_multi(x, *list(range(3)))
        assert x == []

    def test_add_index_no_exist(self):
        lst = add_index_setup()

        add_index(lst, 'idx')

        add_index_check(lst)

    def test_add_index_idx_is_none(self):
        lst = add_index_setup()
        for d in lst:
            d['idx'] = None

        add_index(lst, 'idx')

        add_index_check(lst)

    def test_add_index_some_exist(self):
        lst = add_index_setup()
        lst[1]['idx'] = 1

        add_index(lst, 'idx')

        add_index_check(lst)

    def test_add_index_sort(self):
        lst = [{'value': 1, 'idx': 1},
               {'value': 0},
               {'value': 2}]

        add_index(lst, 'idx')
        add_index_check(lst)

    def test_add_index_idx_out_of_range(self):
        lst = add_index_setup()
        lst[0]['idx'] = 4

        with self.assertRaises(ValueError) as cm:
            add_index(lst, 'idx')
        assert str(cm.exception) == "Some index value are out-of-range: [4]"

    def test_add_index_idx_duplicate(self):
        lst = add_index_setup()
        lst[0]['idx'] = 1
        lst[1]['idx'] = 1

        with self.assertRaises(ValueError) as cm:
            add_index(lst, 'idx')
        assert str(cm.exception) == "Duplicates found in list: [(1, 2)]"

    def test_length_mixin(self):
        class Foo:
            def __len__(self):
                return 123

        class LengthFoo(LengthMixin, Foo):
            pass

        lf = LengthFoo()

        assert lf.length == 123

    def test_length_list(self):
        assert LengthList(range(5)).length == 5

    def test_config_traverse(self):
        cfg = {'foo': [5, 6, 7], 'bar': {'baz': 7, 'qux': 8}}
        assert set(config_traverse(cfg)) == set([(('foo', 0), 5),
                                                 (('foo', 1), 6),
                                                 (('foo', 2), 7),
                                                 (('bar', 'baz'), 7),
                                                 (('bar', 'qux'), 8)])

        cfg = {}
        assert set(config_traverse(cfg)) == set()

    def test_config_set(self):
        cfg = {'foo': [5, 6, 7], 'bar': {'baz': 7, 'qux': 8}}

        config_set(cfg, ('foo', 0), 10)

        assert cfg['foo'][0] == 10

    def test_list_search(self):
        lst = [{'foo': 5, 'bar': 'A'},
               {'foo': 6, 'bar': 'B'},
               {'foo': 7, 'bar': 'C'}]

        assert list_search(lst, 'foo', 6)['bar'] == 'B'

        with self.assertRaises(KeyError):
            list_search(lst, 'foo', 8)


def add_index_setup():
    lst = []
    for i in range(3):
        lst.append({'value': i})
    return lst


def add_index_check(lst):
    assert lst == [{'value': 0, 'idx': 0},
                   {'value': 1, 'idx': 1},
                   {'value': 2, 'idx': 2}]
