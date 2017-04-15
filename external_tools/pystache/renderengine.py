# coding: utf-8

"""
Defines a class responsible for rendering logic.

"""

import re

from pystache.common import is_string
from pystache.parser import parse


def context_get(stack, name, location):
    """
    Find and return a name from a ContextStack instance.

    """
    return stack.get(name, location)


class RenderEngine(object):

    """
    Provides a render() method.

    This class is meant only for internal use.

    As a rule, the code in this class operates on strings where
    possible rather than, say, strings of bytes or markupsafe.Markup.
    This means that strings obtained from "external" sources like partials
    and variable tag values are immediately converted to str (or
    escaped and converted to str) before being operated on further.
    This makes maintaining, reasoning about, and testing the correctness
    of the code much simpler.  In particular, it keeps the implementation
    of this class independent of the API details of one (or possibly more)
    str subclasses (e.g. markupsafe.Markup).

    """

    # TODO: it would probably be better for the constructor to accept
    #   and set as an attribute a single RenderResolver instance
    #   that encapsulates the customizable aspects of converting
    #   strings and resolving partials and names from context.
    def __init__(self, interpolate=None, resolve_context=None, resolve_partial=None):
        """
        Arguments:

          interpolate: the function used to convert variables tag
            values to str. This function takes a raw string, and
            a formatter key.

            If the value being interpolated is a literal, then the
            formatter key is 'literal'. If no formatter is specified
            then the key is ''.

            interpolate should return a 'str' and honor the specified
            formatter.

          resolve_context: the function to call to resolve a name against
            a context stack.  The function should accept two positional
            arguments: a ContextStack instance and a name to resolve.

          resolve_partial: the function to call when loading a partial.
            The function should accept a template name string and return a
            template string of type str (not a subclass).

        """
        self.interpolate = interpolate
        self.resolve_context = resolve_context
        self.resolve_partial = resolve_partial

    # TODO: Rename context to stack throughout this module.

    # From the spec:
    #
    #   When used as the data value for an Interpolation tag, the lambda
    #   MUST be treatable as an arity 0 function, and invoked as such.
    #   The returned value MUST be rendered against the default delimiters,
    #   then interpolated in place of the lambda.
    #
    def fetch_value(self, context, name, location):
        """
        Get a value from the given context as a basestring instance.

        """
        val = self.resolve_context(context, name, location)

        if hasattr(val, '__call__'):
            # Return because _render_value() is already a string.
            return self._render_value(val(), context, location=location)

        return val

    def fetch_section_data(self, context, name, location):
        data = self.resolve_context(context, name, location)

        # From the spec:
        #
        #   If the data is not of a list type, it is coerced into a list
        #   as follows: if the data is truthy (e.g. `!!data == true`),
        #   use a single-element list containing the data, otherwise use
        #   an empty list.
        #
        if not data:
            data = []
        else:
            # The least brittle way to determine whether something
            # supports iteration is by trying to call iter() on it:
            #
            #   http://docs.python.org/library/functions.html#iter
            #
            # It is not sufficient, for example, to check whether the item
            # implements __iter__ () (the iteration protocol).  There is
            # also __getitem__() (the sequence protocol).  In Python 2,
            # strings do not implement __iter__(), but in Python 3 they do.
            try:
                iter(data)
            except TypeError:
                # Then the value does not support iteration.
                data = [data]
            else:
                if is_string(data) or isinstance(data, dict):
                    # Do not treat strings and dicts (which are iterable) as lists.
                    data = [data]
                # Otherwise, treat the value as a list.

        return data

    def _render_value(self, val, context, delimiters=None, location=None):
        """
        Render an arbitrary value.

        """
        if not is_string(val):
            # In case the template is an integer, for example.
            val = str(val)
        if type(val) is not str:
            val = self.interpolate(val, 'literal', location)
        return self.render(val, context, delimiters)

    def render(self, template, context_stack, delimiters=None, name=None):
        """Render a template string, and return as str.

        Arguments:

          template: a template string of type str (but not a proper
            subclass of str).

          context_stack: a ContextStack instance.

        """
        parsed_template = parse(template, delimiters, name)

        return parsed_template.render(self, context_stack)
