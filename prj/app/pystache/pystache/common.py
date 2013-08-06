# coding: utf-8

"""
Exposes functionality needed throughout the project.

"""

from sys import version_info


_STRING_TYPES = (str, bytes)


def is_string(obj):
    """
    Return whether the given object is a bytes or string

    """
    return isinstance(obj, _STRING_TYPES)


# This function was designed to be portable across Python versions -- both
# with older versions and with Python 3 after applying 2to3.
def read(path):
    """
    Return the contents of a text file as a byte string.

    """
    with open(path, 'rb') as f:
        return f.read()


class MissingTags(object):

    """Contains the valid values for Renderer.missing_tags."""

    ignore = 'ignore'
    strict = 'strict'


class PystacheError(Exception):
    """Base class for Pystache exceptions."""
    pass


class TemplateNotFoundError(PystacheError):
    """An exception raised when a template is not found."""
    pass
