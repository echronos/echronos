# encoding: utf-8

"""
Unit tests of loader.py.

"""

import os
import sys
import unittest

from pystache.tests.common import AssertStringMixin, DATA_DIR, SetupDefaults
from pystache import defaults
from pystache.loader import Loader


# We use the same directory as the locator tests for now.
LOADER_DATA_DIR = os.path.join(DATA_DIR, 'locator')


class LoaderTests(unittest.TestCase, AssertStringMixin, SetupDefaults):

    def setUp(self):
        self.setup_defaults()

    def tearDown(self):
        self.teardown_defaults()

    def test_init__extension(self):
        loader = Loader(extension='foo')
        self.assertEqual(loader.extension, 'foo')

    def test_init__extension__default(self):
        # Test the default value.
        loader = Loader()
        self.assertEqual(loader.extension, 'mustache')

    def test_init__file_encoding(self):
        loader = Loader(file_encoding='bar')
        self.assertEqual(loader.file_encoding, 'bar')

    def test_init__file_encoding__default(self):
        file_encoding = defaults.FILE_ENCODING
        try:
            defaults.FILE_ENCODING = 'foo'
            loader = Loader()
            self.assertEqual(loader.file_encoding, 'foo')
        finally:
            defaults.FILE_ENCODING = file_encoding

    def _get_path(self, filename):
        return os.path.join(DATA_DIR, filename)

    # TODO: check the read() unit tests.
    def test_read(self):
        """
        Test read().

        """
        loader = Loader()
        path = self._get_path('ascii.mustache')
        actual = loader.read(path)
        self.assertString(actual, 'ascii: abc')

    def test_read__file_encoding__attribute(self):
        """
        Test read(): file_encoding attribute respected.

        """
        loader = Loader()
        path = self._get_path('non_ascii.mustache')

        self.assertRaises(UnicodeDecodeError, loader.read, path)

        loader.file_encoding = 'utf-8'
        actual = loader.read(path)
        self.assertString(actual, 'non-ascii: é')

    def test_read__encoding__argument(self):
        """
        Test read(): encoding argument respected.

        """
        loader = Loader()
        path = self._get_path('non_ascii.mustache')

        self.assertRaises(UnicodeDecodeError, loader.read, path)

        actual = loader.read(path, encoding='utf-8')
        self.assertString(actual, 'non-ascii: é')

    def test_read__to_unicode__attribute(self):
        """
        Test read(): to_unicode attribute respected.

        """
        loader = Loader()
        path = self._get_path('non_ascii.mustache')

        self.assertRaises(UnicodeDecodeError, loader.read, path)

        #loader.decode_errors = 'ignore'
        #actual = loader.read(path)
        #self.assertString(actual, 'non-ascii: ')

    def test_load_file(self):
        loader = Loader(search_dirs=[DATA_DIR, LOADER_DATA_DIR])
        template = loader.load_file('template.txt')
        self.assertEqual(template, 'Test template file\n')

    def test_load_name(self):
        loader = Loader(search_dirs=[DATA_DIR, LOADER_DATA_DIR],
                        extension='txt')
        template = loader.load_name('template')
        self.assertEqual(template, 'Test template file\n')
