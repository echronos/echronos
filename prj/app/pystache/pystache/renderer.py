# coding: utf-8

"""
This module provides a Renderer class to render templates.

"""

import sys

from pystache import defaults
from pystache.common import TemplateNotFoundError, MissingTags, FormatterNotFoundError
from pystache.context import ContextStack, KeyNotFoundError
from pystache.loader import Loader
from pystache.locator import LocatorNotFoundError
from pystache.parsed import ParsedTemplate
from pystache.renderengine import context_get, RenderEngine
from pystache.specloader import SpecLoader
from pystache.template_spec import TemplateSpec


class Renderer(object):

    """
    A class for rendering mustache templates.

    This class supports several rendering options which are described in
    the constructor's docstring.  Among these, the constructor supports
    passing a custom partial loader.

    Here is an example of rendering a template using a custom partial loader
    that loads partials from a string-string dictionary.

    >>> partials = {'partial': 'Hello, {{thing}}!'}
    >>> renderer = Renderer(partials=partials)
    >>> # We apply print to make the test work in Python 3 after 2to3.
    >>> print(renderer.render('{{>partial}}', {'thing': 'world'}))
    Hello, world!

    """

    def __init__(self, file_encoding=None, string_encoding=None,
                 decode_errors=None, search_dirs=None, file_extension=None,
                 escape=None, partials=None, missing_tags=None):
        """
        Construct an instance.

        Arguments:

          file_encoding: the name of the encoding to use by default when
            reading template files.  All templates are converted to str
            prior to parsing.  Defaults to the package default.

          string_encoding: the name of the encoding to use when converting
            to str any bytes encountered
            during the rendering process.  This name will be passed as the
            encoding argument to the built-in str() function.
            Defaults to the package default.

          decode_errors: the string to pass as the errors argument to the
            built-in function str() when converting byte strings to
            str.  Defaults to the package default.

          search_dirs: the list of directories in which to search when
            loading a template by name or file name.  If given a string,
            the method interprets the string as a single directory.
            Defaults to the package default.

          file_extension: the template file extension.  Pass False for no
            extension (i.e. to use extensionless template files).
            Defaults to the package default.

          partials: an object (e.g. a dictionary) for custom partial loading
            during the rendering process.
                The object should have a get() method that accepts a string
            and returns the corresponding template as a string
                If there is no template with that name,
            the get() method should either return None (as dict.get() does)
            or raise an exception.
                If this argument is None, the rendering process will use
            the normal procedure of locating and reading templates from
            the file system -- using relevant instance attributes like
            search_dirs, file_encoding, etc.

          escape: the function used to escape variable tag values when
            rendering a template.  The function should accept a
            string (or subclass of str) and return an escaped string
            that is again str (or a subclass of str).
                This function need not handle strings of type `bytes` because
            this class will only pass it strings.  The constructor
            assigns this function to the constructed instance's escape()
            method.
                To disable escaping entirely, one can pass `lambda u: u`
            as the escape function, for example.  One may also wish to
            consider using markupsafe's escape function: markupsafe.escape().
            This argument defaults to the package default.

          missing_tags: a string specifying how to handle missing tags.
            If 'strict', an error is raised on a missing tag.  If 'ignore',
            the value of the tag is the empty string.  Defaults to the
            package default.

        """
        if decode_errors is None:
            decode_errors = defaults.DECODE_ERRORS

        if escape is None:
            escape = defaults.TAG_ESCAPE

        if file_encoding is None:
            file_encoding = defaults.FILE_ENCODING

        if file_extension is None:
            file_extension = defaults.TEMPLATE_EXTENSION

        if missing_tags is None:
            missing_tags = defaults.MISSING_TAGS

        if search_dirs is None:
            search_dirs = defaults.SEARCH_DIRS

        if string_encoding is None:
            string_encoding = defaults.STRING_ENCODING

        if isinstance(search_dirs, str):
            search_dirs = [search_dirs]

        self._context = None
        self.decode_errors = decode_errors
        self.file_encoding = file_encoding
        self.file_extension = file_extension
        self.missing_tags = missing_tags
        self.partials = partials
        self.search_dirs = search_dirs
        self.string_encoding = string_encoding
        self.formatters = {}
        self.escape = escape
        self.literal = str

    @property
    def escape(self):
        return self.formatters['']

    @escape.setter
    def escape(self, val):
        self.formatters[''] = val

    @property
    def literal(self):
        return self.formatters['literal']

    @escape.setter
    def literal(self, val):
        self.formatters['literal'] = val

    # This is an experimental way of giving views access to the current context.
    # TODO: consider another approach of not giving access via a property,
    #   but instead letting the caller pass the initial context to the
    #   main render() method by reference.  This approach would probably
    #   be less likely to be misused.
    @property
    def context(self):
        """
        Return the current rendering context [experimental].

        """
        return self._context

    def _interpolate(self, val, formatter_key, location):
        """Convert a value to string.

        """
        try:
            formatter = self.formatters[formatter_key]
        except:
            raise FormatterNotFoundError(formatter_key, location)
        if isinstance(val, bytes):
            val = self._bytes_to_str(val)
        elif not isinstance(val, str):
            val = str(val)
        return formatter(val)

    def _bytes_to_str(self, _bytes):
        """Convert a byte string to str, using string_encoding and decode_errors.

        """
        assert type(_bytes) == bytes
        return str(_bytes, self.string_encoding, self.decode_errors)

    def _make_loader(self):
        """
        Create a Loader instance using current attributes.

        """
        return Loader(file_encoding=self.file_encoding, extension=self.file_extension,
                      search_dirs=self.search_dirs)

    def _make_load_template(self):
        """
        Return a function that loads a template by name.

        """
        loader = self._make_loader()

        def load_template(template_name, location):
            try:
                return loader.load_name(template_name)
            except LocatorNotFoundError as e:
                raise TemplateNotFoundError(str(e), location)

        return load_template

    def _make_load_partial(self):
        """
        Return a function that loads a partial by name.

        """
        if self.partials is None:
            return self._make_load_template()

        # Otherwise, create a function from the custom partial loader.
        partials = self.partials

        def load_partial(name, location):
            # TODO: consider using EAFP here instead.
            #     http://docs.python.org/glossary.html#term-eafp
            #   This would mean requiring that the custom partial loader
            #   raise a KeyError on name not found.
            template = partials.get(name)
            if template is None:
                msg = "Name {} not found in partials: {}".format(repr(name), type(partials))
                raise TemplateNotFoundError(msg, location)

            # RenderEngine requires that the return value be str.
            assert isinstance(template, str)
            return template

        return load_partial

    def _is_missing_tags_strict(self):
        """
        Return whether missing_tags is set to strict.

        """
        val = self.missing_tags

        if val == MissingTags.strict:
            return True
        elif val == MissingTags.ignore:
            return False

        raise Exception("Unsupported 'missing_tags' value: %s" % repr(val))

    def _make_resolve_partial(self):
        """
        Return the resolve_partial function to pass to RenderEngine.__init__().

        """
        load_partial = self._make_load_partial()

        def resolve_partial_add_loc(name, location):
            return load_partial(name, location)

        def resolve_partial_squelch(name, location):
            try:
                return load_partial(name, location)
            except TemplateNotFoundError:
                return ''

        return resolve_partial_add_loc if self._is_missing_tags_strict() else resolve_partial_squelch

    def _make_resolve_context(self):
        """
        Return the resolve_context function to pass to RenderEngine.__init__().

        """
        def resolve_context_squelch(stack, name, location):
            try:
                return context_get(stack, name, location)
            except KeyNotFoundError:
                return ''

        def resolve_context_add_loc(stack, name, location):
            return context_get(stack, name, location)

        return resolve_context_add_loc if self._is_missing_tags_strict() else resolve_context_squelch

    def _make_render_engine(self):
        """
        Return a RenderEngine instance for rendering.

        """
        resolve_context = self._make_resolve_context()
        resolve_partial = self._make_resolve_partial()

        engine = RenderEngine(interpolate=self._interpolate,
                              resolve_context=resolve_context,
                              resolve_partial=resolve_partial)
        return engine

    # TODO: add unit tests for this method.
    def load_template(self, template_name):
        """
        Load a template by name from the file system.

        """
        load_template = self._make_load_template()
        return load_template(template_name, None)

    def _render_object(self, obj, *context, **kwargs):
        """
        Render the template associated with the given object.

        """
        loader = self._make_loader()

        # TODO: consider an approach that does not require using an if
        #   block here.  For example, perhaps this class's loader can be
        #   a SpecLoader in all cases, and the SpecLoader instance can
        #   check the object's type.  Or perhaps Loader and SpecLoader
        #   can be refactored to implement the same interface.
        try:
            if isinstance(obj, TemplateSpec):
                loader = SpecLoader(loader)
                template = loader.load(obj)
            else:
                template = loader.load_object(obj)
        except LocatorNotFoundError as e:
            raise TemplateNotFoundError(str(e), None)

        context = [obj] + list(context)

        return self._render_string(template, '<obj>', *context, **kwargs)

    def render_path(self, template_path, *context, **kwargs):
        """
        Render the template at the given path using the given context.

        Read the render() docstring for more information.

        """
        loader = self._make_loader()
        template = loader.read(template_path)

        return self._render_string(template, template_path, *context, **kwargs)

    def _render_string(self, template, name, *context, **kwargs):
        """
        Render the given template string using the given context.

        """
        assert isinstance(template, str)

        render_func = lambda engine, stack: engine.render(template, stack, name=name)
        return self._render_final(render_func, *context, **kwargs)

    # All calls to render() should end here because it prepares the
    # context stack correctly.
    def _render_final(self, render_func, *context, **kwargs):
        """
        Arguments:

          render_func: a function that accepts a RenderEngine and ContextStack
            instance and returns a template rendering as a string.

        """
        stack = ContextStack.create(*context, **kwargs)
        self._context = stack

        engine = self._make_render_engine()

        return render_func(engine, stack)

    def register_formatter(self, specifier, function):
        """Register a specific function as the formatter for a given specifier."""
        assert callable(function)
        self.formatters[specifier] = function

    def render(self, template, *context, **kwargs):
        """
        Render the given template string, view template, or parsed template.

        Returns a string.

        Prior to rendering, this method will convert a template that is a
        bytes to string using the string_encoding
        and decode_errors attributes.  See the constructor docstring for
        more information.

        Arguments:

          template: a template string that is string or bytes,
            a ParsedTemplate instance, or another object instance.  In the
            final case, the function first looks for the template associated
            to the object by calling this class's get_associated_template()
            method.  The rendering process also uses the passed object as
            the first element of the context stack when rendering.

          *context: zero or more dictionaries, ContextStack instances, or objects
            with which to populate the initial context stack.  None
            arguments are skipped.  Items in the *context list are added to
            the context stack in order so that later items in the argument
            list take precedence over earlier items.

          **kwargs: additional key-value data to add to the context stack.
            As these arguments appear after all items in the *context list,
            in the case of key conflicts these values take precedence over
            all items in the *context list.

        """
        name = None
        if isinstance(template, bytes):
            template = self._bytes_to_str(template)

        if isinstance(template, str):
            return self._render_string(template, name, *context, **kwargs)
        elif isinstance(template, ParsedTemplate):
            render_func = lambda engine, stack: template.render(engine, stack)
            return self._render_final(render_func, name, *context, **kwargs)
        else:
            # Otherwise, we assume the template is an object.
            return self._render_object(template, *context, **kwargs)
