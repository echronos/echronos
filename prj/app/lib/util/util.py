"""A set of generic utility functions and classes.

This module provides a holding space for small chunks of
functionality. All the functions and classess in this module are
candidates for refactor at some time.

"""


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
