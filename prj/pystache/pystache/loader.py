# coding: utf-8

"""
This module provides a Loader class for locating and reading templates.

"""

import os
import sys

from pystache import common
from pystache import defaults
from pystache.locator import Locator


class Loader(object):

    """
    Loads the template associated to a name or user-defined object.

    """

    def __init__(self, file_encoding=None, extension=None, search_dirs=None):
        """
        Construct a template loader instance.

        Arguments:

          extension: the template file extension, without the leading dot.
            Pass False for no extension (e.g. to use extensionless template
            files).  Defaults to the package default.

          file_encoding: the name of the encoding to use when converting file
            contents to str.  Defaults to the package default.

          search_dirs: the list of directories in which to search when loading
            a template by name or file name.  Defaults to the package default.

        """
        if extension is None:
            extension = defaults.TEMPLATE_EXTENSION

        if file_encoding is None:
            file_encoding = defaults.FILE_ENCODING

        if search_dirs is None:
            search_dirs = defaults.SEARCH_DIRS

        self.extension = extension
        self.file_encoding = file_encoding
        # TODO: unit test setting this attribute.
        self.search_dirs = search_dirs

    def _make_locator(self):
        return Locator(extension=self.extension)

    def read(self, path, encoding=None):
        """
        Read the template at the given path, and return it as a string.

        """
        b = common.read(path)

        if encoding is None:
            encoding = self.file_encoding

        return str(b, encoding)

    def load_file(self, file_name):
        """
        Find and return the template with the given file name.

        Arguments:

          file_name: the file name of the template.

        """
        locator = self._make_locator()

        path = locator.find_file(file_name, self.search_dirs)

        return self.read(path)

    def load_name(self, name):
        """
        Find and return the template with the given template name.

        Arguments:

          name: the name of the template.

        """
        locator = self._make_locator()

        path = locator.find_name(name, self.search_dirs)

        return self.read(path)

    # TODO: unit-test this method.
    def load_object(self, obj):
        """
        Find and return the template associated to the given object.

        Arguments:

          obj: an instance of a user-defined class.

          search_dirs: the list of directories in which to search.

        """
        locator = self._make_locator()

        path = locator.find_object(obj, self.search_dirs)

        return self.read(path)
