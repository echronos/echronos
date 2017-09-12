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
import os
import tempfile
import unittest
from pylib.utils import BASE_DIR, LineFilter, update_file, get_release_version, find_path, TOP_DIR
from pylib.components import _sort_typedefs, _sort_by_dependencies, _DependencyNode, _UnresolvableDependencyError
from pylib.release import get_release_configs
from pylib.task import _Review, Task, _InvalidTaskStateError, TaskConfiguration


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

    def test_full_stop_in_reviewer_name(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            round_ = 0
            author = 'john.doe'
            review_file_path = os.path.join(temp_dir, '{}.{}.md'.format(author, round_))
            open(review_file_path, 'w').close()
            review = _Review(review_file_path)
            self.assertEqual(review.author, author)
            self.assertEqual(review.round, round_)

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

    @unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
    def test_task_accepted(self):  # pylint: disable=no-self-use
        # This task was accepted without any rework reviews
        task_dummy_create("test_task_accepted")._check_is_accepted()

    @unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
    def test_rework_is_accepted(self):  # pylint: disable=no-self-use
        # This task had a rework review that was later accepted by its review author
        task_dummy_create("test_rework_is_accepted")._check_is_accepted()

    @unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
    def test_rework_not_accepted(self):
        # This task was erroneously integrated with a rework review not later accepted by its review author
        task = task_dummy_create("test_rework_not_accepted")
        self.assertRaises(_InvalidTaskStateError, task._check_is_accepted)

    @unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
    def test_not_enough_accepted(self):
        # This task was integrated after being accepted by only one reviewer
        # before we placed a hard minimum in the check
        task = task_dummy_create("test_not_enough_accepted")
        self.assertRaises(_InvalidTaskStateError, task._check_is_accepted)

    def test_update_file(self):
        # Ensure the following properties of update_file()
        # - performs the expected line manipulation
        # - leaves a file 100% unmodified when the line filters are set up to have no effect
        # - leaves lines unaffected by line filters 100% unmodified
        # - gracefully handles complex unicode characters
        # - gracefully handles different line endings
        line1 = u'line one: 繁\n'
        line2 = u'line two: ℕ\n'

        for newline in ('\n', '\r', '\r\n'):
            tf_obj = tempfile.NamedTemporaryFile(delete=False)
            tf_obj.write('{}{}'.format(line1.replace('\n', newline), line2.replace('\n', newline)).encode('utf8'))
            tf_obj.close()
            self._test_update_file_on_path(tf_obj.name, line1, line2)
            os.remove(tf_obj.name)

    def _test_update_file_on_path(self, file_path, line1, line2):
        with open(file_path, 'rb') as file_obj:
            original_file_contents = file_obj.read()

        def matches(_, line, __, ___):
            return line.startswith('line two')

        def replace_none(_, line, __, ___):
            return line.replace('line two', 'line two')

        def handle_no_matches(_, __):
            pass

        update_file(file_path, [LineFilter(matches, replace_none, handle_no_matches)])
        with open(file_path, 'rb') as file_obj:
            updated_file_contents = file_obj.read()
        self.assertEqual(original_file_contents, updated_file_contents)

        def replace_number(_, line, __, ___):
            return line.replace('line two', 'line 2')

        update_file(file_path, [LineFilter(matches, replace_number, handle_no_matches)])
        with open(file_path, 'r', encoding='utf8') as file_obj:
            line = file_obj.readline()
            self.assertEqual(line, line1)
            line = file_obj.readline()
            self.assertEqual(line, line2.replace('line two', 'line 2'))

    @unittest.skipUnless(os.path.isdir(os.path.join(BASE_DIR, '.git')), 'Test depends on valid git repo')
    def test_get_release_impact(self):
        cfg = TaskConfiguration(repo_path=BASE_DIR,
                                tasks_path=os.path.join('pm', 'tasks'),
                                description_template_path=None,
                                reviews_path=os.path.join('pm', 'reviews'),
                                mainline_branch='master',
                                manage_release_version=False)
        task = Task(cfg, 'manage_release_version_numbers', checkout=False)
        self.assertEqual(task._get_release_impact(), 'patch')

    def test_get_release_version(self):
        imported_version_str = get_release_configs()[0].version
        rls_cfg_path = find_path('release_cfg.py', TOP_DIR)
        parsed_version_str = '.'.join(str(nmbr) for nmbr in get_release_version(rls_cfg_path))
        self.assertEqual(parsed_version_str, imported_version_str)


# Helper for the pre-integration check tests
def task_dummy_create(task_name):
    cfg = TaskConfiguration(repo_path=BASE_DIR,
                            tasks_path=os.path.join('x_test_data', 'tasks'),
                            description_template_path=None,
                            reviews_path=os.path.join('x_test_data', 'reviews'),
                            mainline_branch='master',
                            manage_release_version=False)
    return Task(cfg, task_name, checkout=False)
