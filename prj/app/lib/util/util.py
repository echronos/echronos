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

"""A set of generic utility functions and classes.

This module provides a holding space for small chunks of
functionality. All the functions and classess in this module are
candidates for refactor at some time.

"""
from operator import itemgetter


def do_nothing(*args, **kwargs):
    """do_nothing may be used as a generic callback
    or handler function that does exactly nothing.

    """


class Singleton:
    """The Singleton class is designed to be used for creating
    unique 'marker' style values. This generally replaces cases
    where object() would be used directly.

    Instead of:
    foo = object()
    ...
    if x is foo:
       # do_something

    Use:
    foo = Singleton('foo')

    The advanage of this approach is when debugging printing a Singleton
    object is much more useful than a raw object.

    """

    def __init__(self, name):
        """Create and set the name of the singleton. 'name' should match
        the name of the variable the object is being bound to. E.g:

        foo = Singleton('foo')

        not:

        bar = Singleton('BAR')

        """
        self.name = name

    def __repr__(self):
        return "<Singleton: {}>".format(self.name)


# Python doesn't have fixed size integers, so bit-manipulation
# can be a bit hairy. s16l provides a simple wrapper that reduces
# error prone shift-and-mask code repeated everywhere.

def s16l(value, n):
    """Shift an unsigned 16-bit value left by n-bits."""
    return (value << n) & 0xffff


def check_unique(lst):
    """Raise exception if the items in the list are not unique."""
    uniq = list(set(lst))
    if len(uniq) != len(lst):
        dups = list((u, lst.count(u)) for u in uniq if lst.count(u) > 1)
        raise ValueError("Duplicates found in list: {}".format(dups))


def remove_multi(lst, *values):
    """Remove multiple values from a list."""
    for value in values:
        lst.remove(value)


def add_index(lst, key):
    """Given a list of dictionaries, add an index to each dictionary.

    The key for the index value is specified as a parameter.

    The dictionaries in the list may already have index values specified.
    In this case, these index values are not modified.

    The list is sorted so that index for each object matches its location in the list.

    If the value of existing keys is incorrect then a ValueError is raised.

    """
    indexes = [d[key] for d in lst if d.get(key) is not None]
    out_of_range = [idx for idx in indexes if idx >= len(lst) or idx < 0]
    if len(out_of_range) > 0:
        raise ValueError("Some index value are out-of-range: {}".format(out_of_range))
    check_unique(indexes)

    # Assign missing indexes to dictionaries without the key.
    new_indexes = list(range(len(lst)))
    remove_multi(new_indexes, *indexes)
    dicts_without_index = (d for d in lst if d.get(key) is None)

    for d, new_index in zip(dicts_without_index, new_indexes):
        d[key] = new_index

    lst.sort(key=itemgetter(key))


class LengthMixin:
    """A mixin that provides classes with a 'length' property.

    It should be possible to use this mixin on any class providing an __len__ method.

    Note: This should only be used in very specific circumstances.
    In most cases it is much more pythonic to use len(...)

    """
    @property
    def length(self):
        return len(self)


class LengthList(list, LengthMixin):
    """A list class that supports a 'length' property.

    Note: as per the LengthMixin this is only to be used in very specific circumstances.

    """


def list_search(lst, key, value):
    """Search a list of dictionaries for the dict where dict[key] == value."""
    try:
        return next(dct for dct in lst if dct[key] == value)
    except StopIteration:
        raise KeyError()


"""A `configuration` is a normal Python dictionary used for storing configuration data.

The values may be any Python value, however any list or dictionary values are treated as holding traversable
configuration data.

A configuration may be accessed via a configuration key, which is a tuple containing a path to a configuration item.

The utility module defines two functions that operate on configurations.

`config_traverse` is a generator that products key, value pairs for all non-container data in the configuration.

`config_set` sets a particular value in the configuration (as indexed by a configuration key).

Future work may investigate encapsulating this functionality within a class, however the current approach allows
for interoperability with existing Python modules that operate on dictionaries.

"""


def config_traverse(cfg):
    """Generate key, value pairs for all non-container elements in a configuration."""
    def traverse(val, key):
        if isinstance(val, list):
            for idx, val_ in enumerate(val):
                yield from traverse(val_, key + (idx, ))
        elif isinstance(val, dict):
            for key_, val_ in val.items():
                yield from traverse(val_, key + (key_, ))
        else:
            yield key, val
    yield from traverse(cfg, tuple())


def config_set(cfg, key, val):
    """Set the value for a given key in the configuration.

    The `key` should be a configuration key, which is a tuple representing the path to an element.

    """
    # Note: This could be implemented pretty nicely as a recursive algortihm, but
    # I've avoided this since there is no tail-call optimisation available.
    assert len(key) > 0

    while len(key) > 1:
        cfg, key = cfg[key[0]], key[1:]

    cfg[key[0]] = val
