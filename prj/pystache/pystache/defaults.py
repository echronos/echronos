# coding: utf-8

"""
This module provides a central location for defining default behavior.

Throughout the package, these defaults take effect only when the user
does not otherwise specify a value.

"""

from html import escape
import os
import sys

from pystache.common import MissingTags


# How to handle encoding errors when decoding bytes to str.
#
# This value is passed as the "errors" argument to Python's built-in
# str() function.
#
DECODE_ERRORS = 'strict'

# The name of the encoding to use when converting any bytes to str
STRING_ENCODING = sys.getdefaultencoding()

# The name of the encoding to use when converting file contents to str.
# This default takes precedence over the STRING_ENCODING default for
# strings that arise from files.
FILE_ENCODING = sys.getdefaultencoding()

# The delimiters to start with when parsing.
DELIMITERS = ('{{', '}}')

# The formatter delimiter
FORMAT_DELIMITER = '|'

# How to handle missing tags when rendering a template.
MISSING_TAGS = MissingTags.ignore

# The starting list of directories in which to search for templates when
# loading a template by file name.
SEARCH_DIRS = [os.curdir]  # i.e. ['.']

# The escape function to apply to strings that require escaping when
# rendering templates (e.g. for tags enclosed in double braces).
# Only strings (not bytes) will be passed to this function.
#
# The quote=True argument causes double but not single quotes to be escaped
# in Python 3.1 and earlier, and both double and single quotes to be
# escaped in Python 3.2 and later:
#
#   http://docs.python.org/library/cgi.html#cgi.escape
#   http://docs.python.org/dev/library/html.html#html.escape
#
TAG_ESCAPE = lambda u: escape(u, quote=True)

# The default template extension, without the leading dot.
TEMPLATE_EXTENSION = 'mustache'
