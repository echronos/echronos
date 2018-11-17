#
# eChronos Real-Time Operating System
# Copyright (c) 2017, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#

# pylint: disable=protected-access
import itertools
import unittest
from pylib.components import _sort_typedefs, _sort_by_dependencies, _DependencyNode, _UnresolvableDependencyError


class TestCase(unittest.TestCase):
    def test_empty(self):
        """Test whether an empty test can be run at all given the test setup in x.py."""
        pass

    def test_sort_typedefs(self):
        typedefs = ['typedef uint8_t foo;',
                    'typedef foo bar;',
                    'typedef bar baz;']
        expected = '\n'.join(typedefs)
        for permutation in itertools.permutations(typedefs):
            self.assertEqual(_sort_typedefs('\n'.join(permutation)), expected)

    def test_resolve_dependencies(self):
        node_a = _DependencyNode(('a',), ('b', 'c'))
        node_b = _DependencyNode(('b',), ('c',))
        node_c = _DependencyNode(('c',), ())
        nodes = (node_a, node_b, node_c)
        output = list(_sort_by_dependencies(nodes))
        self.assertEqual(output, [node_c, node_b, node_a])

    def test_resolve_unresolvable_dependencies(self):
        node_a = _DependencyNode(('a',), ('b',))
        node_b = _DependencyNode(('b',), ('c',))
        nodes = (node_a, node_b)

        def test_func(nds):
            list(_sort_by_dependencies(nds))

        self.assertRaises(_UnresolvableDependencyError, test_func, nodes)

    def test_resolve_cyclic_dependencies(self):
        node_a = _DependencyNode(('a',), ())
        node_b = _DependencyNode(('b',), ('a',))
        node_c = _DependencyNode(('c',), ('b', 'd'))
        node_d = _DependencyNode(('d',), ('c',))
        nodes = (node_a, node_b, node_c, node_d)

        def test_func(nds):
            list(_sort_by_dependencies(nds))

        self.assertRaises(_UnresolvableDependencyError, test_func, nodes)
        output = list(_sort_by_dependencies(nodes, ignore_cyclic_dependencies=True))
        self.assertEqual(sorted(output), sorted(nodes))
