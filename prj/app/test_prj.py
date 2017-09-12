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

# pylint: disable=too-many-public-methods
import os
import sys
import tempfile
import unittest
from xml.parsers.expat import ExpatError
from prj import get_command_line_arguments, Project, valid_entity_name, System
from util.xml import SystemParseError, xml_parse_file_with_includes, xml_parse_string, xml_parse_file, xml2dict,\
    single_text_child, dict_has_keys, check_schema_is_valid, SchemaInvalidError, list_all_equal,\
    asdict, check_ident, get_attribute, ensure_unique_tag_names, element_children, ensure_all_children_named

_PRJ_APP_DIR = os.path.dirname(__file__)


class TestCase(unittest.TestCase):
    def test_dict_has_keys(self):
        test_dict = {'foo': 37, 'bar': 25}
        self.assertTrue(dict_has_keys(test_dict, 'foo'))
        self.assertFalse(dict_has_keys(test_dict, 'baz'))
        self.assertTrue(dict_has_keys(test_dict, 'foo', 'bar'))

    def test_schema_is_valid(self):
        check_schema_is_valid(None)
        good = {
            'type': 'dict',
            'name': 'module',
            'dict_type': (
                [{'type': 'string', 'name': 'target', 'default': ''}],
                []
            )
        }
        check_schema_is_valid(good)

        no_name = {
            'type': 'dict',
            'dict_type': (
                [{'type': 'string', 'name': 'target', 'default': ''}],
                []
            )
        }
        self.assertRaises(SchemaInvalidError, check_schema_is_valid, no_name)

        bad_dict_type = {
            'name': 'module',
            'type': 'dict',
            'dict_type':
            {
                'foo': {'type': 'string', 'name': 'target', 'default': ''}
            }
        }
        self.assertRaises(SchemaInvalidError, check_schema_is_valid, bad_dict_type)

        invalid_nested = {
            'type': 'dict',
            'name': 'module',
            'dict_type': (
                [{'name': 'target', 'default': ''}],
                []
            )
        }
        self.assertRaises(SchemaInvalidError, check_schema_is_valid, invalid_nested)

        list_no_type = {
            'type': 'list',
            'name': 'foo'
        }
        self.assertRaises(SchemaInvalidError, check_schema_is_valid, list_no_type)

        list_nested_bad_dict = {
            'type': 'list',
            'name': 'foo',
            'list_type': {'type': 'dict_type', 'name': 'foo'}
        }
        self.assertRaises(SchemaInvalidError, check_schema_is_valid, list_nested_bad_dict)

    def test_list_all_equal(self):
        self.assertTrue(list_all_equal("11111111"))
        self.assertFalse(list_all_equal("11111110"))

    def test_single_text_child(self):
        dom = xml_parse_string('<x>X</x>')
        self.assertEqual(single_text_child(dom), 'X')

    def test_single_text_child_a(self):
        dom = xml_parse_string('<x></x>')
        self.assertEqual(single_text_child(dom), '')

    def test_xml2dict_simple_list(self):
        xml = """<list>
               <li>foo</li>
               <li>bar</li>
               <li>baz</li>
              </list>"""
        expected = ['foo', 'bar', 'baz']
        self.assertEqual(xml2dict(xml_parse_string(xml)), expected)

    def test_xml2dict_simple_dict(self):
        xml = """<dict>
          <a>foo</a>
          <b>bar</b>
          <c>baz</c>
         </dict>"""
        expected = {'a': 'foo', 'b': 'bar', 'c': 'baz'}
        self.assertEqual(xml2dict(xml_parse_string(xml)), expected)

    def test_xml2dict_single_item_list(self):
        xml = """<list>
          <a>foo</a>
         </list>"""
        expected = ['foo']
        self.assertEqual(xml2dict(xml_parse_string(xml)), expected)

    def test_xml2dict_empty_node(self):
        xml = """<list></list>"""
        expected = ''
        self.assertEqual(xml2dict(xml_parse_string(xml)), expected)

    def test_schema_default_none(self):
        test_xml = "<foo></foo>"
        schema = {
            'type': 'dict',
            'name': 'foo',
            'dict_type': ([{'type': 'string',
                            'name': 'foo'}], [])
        }
        with self.assertRaises(SystemParseError):
            xml2dict(xml_parse_string(test_xml), schema)

    def test_schema_default_value(self):
        schema = {
            'type': 'dict',
            'name': 'foo',
            'dict_type': ([{'type': 'string',
                            'name': 'bar',
                            'default': 'FOO'}], [])
        }
        self.assertEqual(xml2dict(xml_parse_string("<foo></foo>"), schema), {'bar': 'FOO'})
        self.assertEqual(xml2dict(xml_parse_string("<foo><bar>BAZ</bar></foo>"), schema), {'bar': 'BAZ'})

    def test_xml2dict_length_prop(self):
        test_xml = "<list><li>foo</li><li>bar</li><li>baz</li></list>"
        test_dict = xml2dict(xml_parse_string(test_xml))
        self.assertEqual(test_dict.length, 3)

    def test_xml2dict_autoindex(self):  # pylint: disable=no-self-use
        test_xml = "<list><x><name>foo</name></x><x><name>bar</name></x><x><name>x</name></x></list>"
        schema = {
            'type': 'list',
            'name': 'list',
            'auto_index_field': 'idx',
            'list_type': {'type': 'dict',
                          'name': 'x',
                          'dict_type': ([{'name': 'name', 'type': 'string'}], [])}
        }
        xml2dict(xml_parse_string(test_xml), schema)

    def test_xml2dict_bool(self):
        schema = {
            'type': 'bool',
            'name': 'b'
        }

        test_xml = "<b>true</b>"
        self.assertTrue(xml2dict(xml_parse_string(test_xml), schema))

        test_xml = "<b>false</b>"
        self.assertFalse(xml2dict(xml_parse_string(test_xml), schema))

        test_xml = "<b>TrUe</b>"
        self.assertTrue(xml2dict(xml_parse_string(test_xml), schema))

        with self.assertRaises(SystemParseError):
            test_xml = "<b>bad</b>"
            self.assertTrue(xml2dict(xml_parse_string(test_xml), schema))

    def test_xml2dict_optional(self):
        schema = {
            'type': 'dict',
            'name': 'x',
            'dict_type': ([{'type': 'bool', 'name': 'b'}], [])
        }

        test_xml = "<x><b>true</b></x>"
        test_dict = xml2dict(xml_parse_string(test_xml), schema)
        self.assertTrue(test_dict['b'])

        test_xml = "<x></x>"
        with self.assertRaises(SystemParseError):
            test_dict = xml2dict(xml_parse_string(test_xml), schema)

        # Make the child element optional instead.
        schema['dict_type'][0][0]['optional'] = True
        test_dict = xml2dict(xml_parse_string(test_xml), schema)
        self.assertIsNone(test_dict['b'])

    def test_xml2dict_ident_error(self):
        """Ensure that exceptions raised while parsing bad idents include location information."""
        test_xml = "<foo>_bad</foo>"
        schema = {'type': 'ident', 'name': 'foo'}
        with self.assertRaises(SystemParseError) as exc:
            xml2dict(xml_parse_string(test_xml), schema)
        self.assertIn('<string>:1.0', str(exc.exception))

    def test_xml2dict_object(self):
        schema = {
            'type': 'dict',
            'name': 'foo',
            'dict_type': (
                [{'type': 'list',
                  'name': 'bars',
                  'list_type': {'type': 'dict',
                                'name': 'bar',
                                'dict_type': ([{'name': 'name', 'type': 'string'}], [])}},
                 {'type': 'list',
                  'name': 'bazs',
                  'list_type': {'type': 'dict',
                                'name': 'baz',
                                'dict_type': ([{'name': 'name', 'type': 'string'},
                                               {'name': 'bar', 'type': 'object', 'object_group': 'bars'}], [])}}],
                [])
        }
        test_xml = """<foo>
<bars>
 <bar><name>A</name></bar>
 <bar><name>B</name></bar>
</bars>
<bazs>
 <baz><name>X</name><bar>A</bar></baz>
</bazs>
</foo>
"""
        parsed = xml2dict(xml_parse_string(test_xml), schema)
        self.assertEqual(parsed['bazs'][0]['bar']['name'], 'A')

        bad_xml = """<foo>
<bars>
 <bar><name>A</name></bar>
 <bar><name>B</name></bar>
</bars>
<bazs>
 <baz><name>X</name><bar>AA</bar></baz>
</bazs>
</foo>
"""
        with self.assertRaises(SystemParseError):
            parsed = xml2dict(xml_parse_string(bad_xml), schema)

    def test_asdict_key(self):
        dict_a = {'foo': 1}
        dict_b = {'foo': 2}
        self.assertEqual(asdict([dict_a, dict_b], key='foo'), {1: dict_a, 2: dict_b})

    def test_asdict_attr(self):
        class Simple(object):
            def __init__(self, test):
                self.test = test

            def __repr__(self):
                return "<Simple: {}>".format(self.test)

        simple_a = Simple(1)
        simple_b = Simple(2)

        self.assertEqual(asdict([simple_a, simple_b], attr='test'), {1: simple_a, 2: simple_b})

    def test_get_attribute_normal(self):
        dom = xml_parse_string('<foo x="X"/>')
        self.assertEqual(get_attribute(dom, 'x'), 'X')
        self.assertEqual(get_attribute(dom, 'x', None), 'X')

    def test_get_attribute_default(self):
        dom = xml_parse_string('<foo/>')
        self.assertIsNone(get_attribute(dom, 'x', None))
        expected_value = '5'
        self.assertIs(get_attribute(dom, 'x', expected_value), expected_value)

    def test_get_attribute_error(self):
        dom = xml_parse_string('<foo/>')
        self.assertRaises(SystemParseError, get_attribute, dom, 'x')

    def test_xml_parse_string_error(self):
        try:
            xml_parse_string("malformed", 'mal', start_line=4)
            assert False
        except ExpatError as exc:
            self.assertEqual(exc.path, 'mal')
            self.assertEqual(exc.lineno, 5)

    def test_xml_parse_string(self):
        dom = xml_parse_string('<x/>', 'mal', start_line=4)
        self.assertEqual(dom.tagName, 'x')
        self.assertEqual(dom.ownerDocument.firstChild, dom)
        self.assertEqual(dom.ownerDocument.start_line, 4)
        self.assertEqual(dom.ownerDocument.path, 'mal')

    def test_ensure_unique_tag_names_unique(self):  # pylint: disable=no-self-use
        unique = "<foo><bar /><baz /><qux /></foo>"
        ensure_unique_tag_names(element_children(xml_parse_string(unique)))

    def test_ensure_unique_tag_names_non_unique(self):
        non_unique = "<foo><bar /><baz /><bar /></foo>"
        self.assertRaises(SystemParseError, ensure_unique_tag_names, element_children(xml_parse_string(non_unique)))

    def test_ensure_all_children_named_success(self):  # pylint: disable=no-self-use
        xml = "<foo><bar /><bar /><bar /></foo>"
        ensure_all_children_named(xml_parse_string(xml), 'bar')

    def test_ensure_all_children_named_multiple_success(self):  # pylint: disable=no-self-use
        xml = "<foo><bar /><bar /><bar /><baz /></foo>"
        ensure_all_children_named(xml_parse_string(xml), 'bar', 'baz')

    def test_ensure_all_children_named_error(self):
        xml = "<foo><bar /><baz /><bar /></foo>"
        self.assertRaises(SystemParseError, ensure_all_children_named, xml_parse_string(xml), 'bar')

    def test_valid_entity_name(self):
        self.assertTrue(valid_entity_name("foo.bar"))
        self.assertFalse(valid_entity_name("foo/bar"))
        self.assertFalse(valid_entity_name("foo\\bar"))
        self.assertFalse(valid_entity_name("foo\\bar/baz"))

    def test_project_find(self):
        project = Project(None, search_paths=[os.path.join(_PRJ_APP_DIR, 'test_data', 'path1')])
        eg_system = project.find('example')
        self.assertIsInstance(eg_system, System)

        project = Project(None, search_paths=[
            os.path.join(_PRJ_APP_DIR, 'test_data', 'path1', 'foo', 'bar', 'baz'),
        ])
        self.assertIsInstance(project.find('qux'), System)

        project = Project(None, search_paths=[
            os.path.join(_PRJ_APP_DIR, 'test_data', 'path1', 'foo', 'bar'),
        ])
        self.assertIsInstance(project.find('baz.qux'), System)

        project = Project(None, search_paths=[
            os.path.join(_PRJ_APP_DIR, 'test_data', 'path1', 'foo'),
        ])
        self.assertIsInstance(project.find('bar.baz.qux'), System)

    def test_xml_parse_file_with_includes_without_include(self):
        prx_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        prx_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
        prx_file.write(prx_xml)
        prx_file.close()
        try:
            result = xml_parse_file_with_includes(prx_file.name)
            result_of_xml_parse_file = xml_parse_file(prx_file.name)
            self.assertEqual(result.toxml(), result_of_xml_parse_file.toxml())
        finally:
            os.remove(prx_file.name)

    def test_xml_parse_file_with_includes(self):
        included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root> <newelement1 />   <newelement2> <newelement3/> </newelement2></include_root>"""
        prx_template = """<?xml version="1.0" encoding="UTF-8" ?>
<system>{}
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        prx_with_include_xml = prx_template.format('<include file="{}" />')

        result = check_xml_parse_file_with_includes_with_xml(prx_with_include_xml, included_xml)

        result_dom = xml_parse_string(result.toprettyxml())
        new_el1s = result_dom.getElementsByTagName('newelement1')
        self.assertEqual(len(new_el1s), 1)
        new_el1 = new_el1s[0]
        self.assertEqual(new_el1.parentNode, result_dom)

        new_el2s = result_dom.getElementsByTagName('newelement2')
        self.assertEqual(len(new_el2s), 1)
        new_el2 = new_el2s[0]
        self.assertEqual(new_el2.parentNode, result_dom)

        new_el3s = result_dom.getElementsByTagName('newelement3')
        self.assertEqual(len(new_el3s), 1)
        new_el3 = new_el3s[0]
        self.assertEqual(new_el3.parentNode, new_el2)

    def test_xml_parse_file_with_includes__absolute(self):  # pylint: disable=no-self-use
        check_xml_parse_file_with_includes__absolute_relative(absolute=True)

    def test_xml_parse_file_with_includes__relative(self):  # pylint: disable=no-self-use
        check_xml_parse_file_with_includes__absolute_relative(absolute=False)

    def test_xml_parse_file_with_includes__include_paths(self):
        try:
            included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root><included_element /></include_root>"""
            main_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
</system>"""
            expected_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <included_element />
</system>"""

            include_dirs = [tempfile.TemporaryDirectory() for _ in range(3)]
            included_dir = include_dirs[1]
            main_dir = tempfile.TemporaryDirectory()

            included_file = tempfile.NamedTemporaryFile(mode='w', delete=False, dir=included_dir.name)
            included_file.write(included_xml)
            included_file.close()

            with open(os.path.join(include_dirs[2].name, os.path.basename(included_file.name)), 'w') as empty_file:
                empty_file.write("""<?xml version="1.0" encoding="UTF-8" ?><include_root></include_root>""")

            main_xml = main_xml.format(os.path.basename(included_file.name))
            main_file = tempfile.NamedTemporaryFile(mode='w', delete=False, dir=main_dir.name)
            main_file.write(main_xml)
            main_file.close()

            result_dom = xml_parse_file_with_includes(main_file.name, [dir_path.name for dir_path in include_dirs])
            expected_dom = xml_parse_string(expected_xml)

            self.assertEqual(result_dom.toxml(), expected_dom.toxml())

        finally:
            for include_dir in include_dirs:
                include_dir.cleanup()

    def test_xml_parse_file_with_includes__nested(self):
        nested_included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root><nested_element /></include_root>"""
        included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root><included_element_1 /> <include file="{}" /> <included_element_2/></include_root>"""
        system_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        expected_xml = """<system>
  <included_element_1 /> <nested_element /> <included_element_2/>
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""

        result_dom = check_xml_parse_file_with_includes_with_xml(system_xml, included_xml, nested_included_xml)
        expected_dom = xml_parse_string(expected_xml)

        self.assertEqual(result_dom.toxml(), expected_dom.toxml())

    def test_xml_parse_file_with_includes__include_as_root_element(self):
        including_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include file="{}" />"""
        included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root>
  <foo />
</include_root>"""
        self.assertRaises(SystemParseError, check_xml_parse_file_with_includes_with_xml, including_xml, included_xml)

    def test_xml_parse_file_with_includes__invalid_path(self):
        xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="/123/abc/!#$" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        self.assertRaises(SystemParseError, check_xml_parse_file_with_includes_with_xml, xml)

    def test_xml_parse_file_with_includes__missing_path_attribute(self):
        xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        self.assertRaises(SystemParseError, check_xml_parse_file_with_includes_with_xml, xml)

    def test_xml_parse_file_with_includes__child_elements(self):
        xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file=".">
    <foo />
  </include>
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        self.assertRaises(SystemParseError, check_xml_parse_file_with_includes_with_xml, xml)

    def test_xml_parse_file_with_includes__wrong_name_of_included_root_element(self):
        xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
        self.assertRaises(SystemParseError, check_xml_parse_file_with_includes_with_xml, xml,
                          """<?xml version="1.0" encoding="UTF-8" ?><foo />""")

    def test_xml_parse_file_with_includes__empty_included_root_element(self):  # pylint: disable=no-self-use
        check_xml_parse_file_with_includes_with_xml("""<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>""", """<?xml version="1.0" encoding="UTF-8" ?><include_root />""")

    def test_xml_include_paths(self):
        sys.argv[1:] = ['--prx-inc-path', 'a', '--prx-inc-path', 'b', 'gen', 'foo']
        args = get_command_line_arguments()
        self.assertEqual(args.prx_inc_path, ['a', 'b'])

        with tempfile.TemporaryDirectory() as temp_dir:
            project_file_path = os.path.join(temp_dir, 'project.prj')
            with open(project_file_path, 'w') as file_obj:
                file_obj.write('''<?xml version="1.0" encoding="UTF-8" ?>
<project>
  <prx-include-path>1</prx-include-path>
  <prx-include-path>2</prx-include-path>
</project>''')

            project = Project(project_file_path)
            self.assertEqual(project._prx_include_paths, ['1', '2'])  # pylint: disable=protected-access

            project = Project(project_file_path, None, args.prx_inc_path)
            self.assertEqual(project._prx_include_paths, ['1', '2', 'a', 'b'])  # pylint: disable=protected-access

    def test_check_ident(self):
        check_ident('foo_bar_123')
        with self.assertRaises(ValueError):
            check_ident('fooFbar')
        with self.assertRaises(ValueError):
            check_ident('Foobar')
        with self.assertRaises(ValueError):
            check_ident('foo_%_')


def check_xml_parse_file_with_includes_with_xml(xml, included_xml=None, nested_included_xml=None):
    if nested_included_xml:
        nested_included_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
        nested_included_file.write(nested_included_xml)
        nested_included_file.close()
        included_xml = included_xml.format(nested_included_file.name)

    if included_xml:
        included_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
        included_file.write(included_xml)
        included_file.close()
        xml = xml.format(included_file.name)

    prx_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
    prx_file.write(xml)
    prx_file.close()

    try:
        return xml_parse_file_with_includes(prx_file.name)
    finally:
        os.remove(prx_file.name)
        if included_xml:
            os.remove(included_file.name)
        if nested_included_xml:
            os.remove(nested_included_file.name)


def check_xml_parse_file_with_includes__absolute_relative(absolute):
    try:
        included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root><included_element /></include_root>"""
        main_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
</system>"""
        expected_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <included_element />
</system>"""

        included_dir = tempfile.TemporaryDirectory()
        main_dir = tempfile.TemporaryDirectory()

        included_file = tempfile.NamedTemporaryFile(mode='w', delete=False, dir=included_dir.name)
        included_file.write(included_xml)
        included_file.close()

        with open(os.path.join(main_dir.name, os.path.basename(included_file.name)), 'w') as empty_file:
            empty_file.write("""<?xml version="1.0" encoding="UTF-8" ?><include_root></include_root>""")

        if absolute:
            included_path = os.path.abspath(included_file.name)
        else:
            included_path = os.path.join('..', os.path.basename(included_dir.name),
                                         os.path.basename(included_file.name))
        main_xml = main_xml.format(included_path)
        main_file = tempfile.NamedTemporaryFile(mode='w', delete=False, dir=main_dir.name)
        main_file.write(main_xml)
        main_file.close()

        result_dom = xml_parse_file_with_includes(main_file.name)
        expected_dom = xml_parse_string(expected_xml)

        assert result_dom.toxml() == expected_dom.toxml()

    finally:
        for include_dir in [included_dir, main_dir]:
            include_dir.cleanup()
