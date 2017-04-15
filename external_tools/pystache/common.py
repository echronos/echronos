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
    def __init__(self, location):
        self.location = location

    def __str__(self):
        return self.msg


class TemplateNotFoundError(PystacheError):
    """An exception raised when a template is not found."""
    def __init__(self, msg, location):
        super().__init__(location)
        self.msg = msg


class FormatterNotFoundError(PystacheError):
    """An exception raised when a formatter is not found."""
    def __init__(self, formatter_key, location):
        super().__init__(location)
        self.formatter_key = formatter_key

    def __str__(self):
        return "Formatter key '{}' not found".format(self.formatter_key)


class ParsingError(PystacheError):
    """An exception raised during parsing."""
    def __init__(self, msg, location):
        super().__init__(location)
        self.msg = msg
