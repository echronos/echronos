# coding: utf-8

"""
Exposes a class that represents a parsed (or compiled) template.

"""


class ParsedTemplate(object):

    def __init__(self):
        self._parse_tree = []

    def __repr__(self):
        return repr(self._parse_tree)

    def add(self, node):
        """
        Arguments:

          node: a string or node object instance.  A node object
            instance must have a `render(engine, stack)` method that
            accepts a RenderEngine instance and a ContextStack instance and
            returns a string.

        """
        self._parse_tree.append(node)

    def render(self, engine, context):
        """
        Returns: a string.

        """
        def get_str(val):
            return val if type(val) is str else val.render(engine, context)
        parts = list(map(get_str, self._parse_tree))
        s = ''.join(parts)

        return str(s)
