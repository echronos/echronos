#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that these additional
# terms apply under section 7:
#
#   No right, title or interest in or to any trade mark, service mark, logo or
#   trade name of of National ICT Australia Limited, ABN 62 102 206 173
#   ("NICTA") or its licensors is granted. Modified versions of the Program
#   must be plainly marked as such, and must not be distributed using
#   "eChronos" as a trade mark or product name, or misrepresented as being the
#   original Program.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
#

import os
import sys
import tempfile
from xml.parsers.expat import ExpatError
from prj import get_command_line_arguments, Project, valid_entity_name, System
from util.xml import SystemParseError, xml_parse_file_with_includes, xml_parse_string, xml_parse_file, xml2dict,\
    single_text_child, dict_has_keys, check_schema_is_valid, SchemaInvalidError, list_all_equal,\
    asdict, check_ident, get_attribute, ensure_unique_tag_names, element_children, ensure_all_children_named
from nose.tools import assert_raises, raises

_PRJ_APP_DIR = os.path.dirname(__file__)


def test_dict_has_keys():
    test_dict = {'foo': 37, 'bar': 25}
    assert dict_has_keys(test_dict, 'foo')
    assert not dict_has_keys(test_dict, 'baz')
    assert dict_has_keys(test_dict, 'foo', 'bar')


def test_schema_is_valid():
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
    assert_raises(SchemaInvalidError, check_schema_is_valid, no_name)

    bad_dict_type = {
        'name': 'module',
        'type': 'dict',
        'dict_type':
        {
            'foo': {'type': 'string', 'name': 'target', 'default': ''}
        }
    }
    assert_raises(SchemaInvalidError, check_schema_is_valid, bad_dict_type)

    invalid_nested = {
        'type': 'dict',
        'name': 'module',
        'dict_type': (
            [{'name': 'target', 'default': ''}],
            []
        )
    }
    assert_raises(SchemaInvalidError, check_schema_is_valid, invalid_nested)

    list_no_type = {
        'type': 'list',
        'name': 'foo'
    }
    assert_raises(SchemaInvalidError, check_schema_is_valid, list_no_type)

    list_nested_bad_dict = {
        'type': 'list',
        'name': 'foo',
        'list_type': {'type': 'dict_type', 'name': 'foo'}
    }
    assert_raises(SchemaInvalidError, check_schema_is_valid, list_nested_bad_dict)


def test_list_all_equal():
    assert list_all_equal("11111111")
    assert not list_all_equal("11111110")


def test_single_text_child():
    dom = xml_parse_string('<x>X</x>')
    assert single_text_child(dom) == 'X'


def test_single_text_child_a():
    dom = xml_parse_string('<x></x>')
    assert single_text_child(dom) == ''


def test_xml2dict():
    tests = [
        ("Simple List",
         """<list>
           <li>foo</li>
           <li>bar</li>
           <li>baz</li>
          </list>""",
         ['foo', 'bar', 'baz']),
        ("Simple Dict",
         """<dict>
           <a>foo</a>
           <b>bar</b>
           <c>baz</c>
          </dict>""",
         {'a': 'foo', 'b': 'bar', 'c': 'baz'}),
        ("Single item list",
         """<list>
           <a>foo</a>
          </list>""",
         ['foo']),
        ("Empty node",
         """<list></list>""",
         ''),
    ]

    def check(xml, result):
        assert xml2dict(xml_parse_string(xml)) == result

    for name, xml, result in tests:
        yield name, check, xml, result


def test_schema_default_none():
    test_xml = "<foo></foo>"
    schema = {
        'type': 'dict',
        'name': 'foo',
        'dict_type': ([{'type': 'string',
                        'name': 'foo'}], [])
    }
    with assert_raises(SystemParseError):
        xml2dict(xml_parse_string(test_xml), schema)


def test_schema_default_value():
    schema = {
        'type': 'dict',
        'name': 'foo',
        'dict_type': ([{'type': 'string',
                        'name': 'bar',
                        'default': 'FOO'}], [])
    }
    assert xml2dict(xml_parse_string("<foo></foo>"), schema) == {'bar': 'FOO'}
    assert xml2dict(xml_parse_string("<foo><bar>BAZ</bar></foo>"), schema) == {'bar': 'BAZ'}


def test_xml2dict_length_prop():
    test_xml = "<list><li>foo</li><li>bar</li><li>baz</li></list>"
    test_dict = xml2dict(xml_parse_string(test_xml))
    assert test_dict.length == 3


def test_xml2dict_autoindex():
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


def test_xml2dict_bool():
    schema = {
        'type': 'bool',
        'name': 'b'
    }

    test_xml = "<b>true</b>"
    assert xml2dict(xml_parse_string(test_xml), schema)

    test_xml = "<b>false</b>"
    assert not xml2dict(xml_parse_string(test_xml), schema)

    test_xml = "<b>TrUe</b>"
    assert xml2dict(xml_parse_string(test_xml), schema)

    with assert_raises(SystemParseError):
        test_xml = "<b>bad</b>"
        assert xml2dict(xml_parse_string(test_xml), schema)


def test_xml2dict_optional():
    schema = {
        'type': 'dict',
        'name': 'x',
        'dict_type': ([{'type': 'bool', 'name': 'b'}], [])
    }

    test_xml = "<x><b>true</b></x>"
    test_dict = xml2dict(xml_parse_string(test_xml), schema)
    assert test_dict['b']

    test_xml = "<x></x>"
    with assert_raises(SystemParseError):
        test_dict = xml2dict(xml_parse_string(test_xml), schema)

    # Make the child element optional instead.
    schema['dict_type'][0][0]['optional'] = True
    test_dict = xml2dict(xml_parse_string(test_xml), schema)
    assert test_dict['b'] is None


def test_xml2dict_ident_error():
    """Ensure that exceptions raised while parsing bad idents include location information."""
    test_xml = "<foo>_bad</foo>"
    schema = {'type': 'ident', 'name': 'foo'}
    with assert_raises(SystemParseError) as exc:
        xml2dict(xml_parse_string(test_xml), schema)
    assert '<string>:1.0' in str(exc.exception)


def test_xml2dict_object():
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
    assert parsed['bazs'][0]['bar']['name'] == 'A'

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
    with assert_raises(SystemParseError):
        parsed = xml2dict(xml_parse_string(bad_xml), schema)


def test_asdict_key():
    dict_a = {'foo': 1}
    dict_b = {'foo': 2}
    assert asdict([dict_a, dict_b], key='foo') == {1: dict_a, 2: dict_b}


def test_asdict_attr():
    class Simple(object):
        def __init__(self, foo):
            self.foo = foo

        def __repr__(self):
            return "<Simple: {}>".format(self.foo)

    simple_a = Simple(1)
    simple_b = Simple(2)

    assert asdict([simple_a, simple_b], attr='foo') == {1: simple_a, 2: simple_b}


def test_get_attribute_normal():
    dom = xml_parse_string('<foo x="X"/>')
    assert get_attribute(dom, 'x') == 'X'
    assert get_attribute(dom, 'x', None) == 'X'


def test_get_attribute_default():
    dom = xml_parse_string('<foo/>')
    assert get_attribute(dom, 'x', None) is None
    assert get_attribute(dom, 'x', '5') is '5'


@raises(SystemParseError)
def test_get_attribute_error():
    dom = xml_parse_string('<foo/>')
    get_attribute(dom, 'x')


def test_xml_parse_string_error():
    try:
        xml_parse_string("malformed", 'mal', start_line=4)
        assert False
    except ExpatError as exc:
        assert exc.path == 'mal'
        assert exc.lineno == 5


def test_xml_parse_string():
    dom = xml_parse_string('<x/>', 'mal', start_line=4)
    assert dom.tagName == 'x'
    assert dom.ownerDocument.firstChild == dom
    assert dom.ownerDocument.start_line == 4
    assert dom.ownerDocument.path == 'mal'


def test_ensure_unique_tag_names_unique():
    unique = "<foo><bar /><baz /><qux /></foo>"
    ensure_unique_tag_names(element_children(xml_parse_string(unique)))


@raises(SystemParseError)
def test_ensure_unique_tag_names_non_unique():
    non_unique = "<foo><bar /><baz /><bar /></foo>"
    ensure_unique_tag_names(element_children(xml_parse_string(non_unique)))


def test_ensure_all_children_named_success():
    xml = "<foo><bar /><bar /><bar /></foo>"
    ensure_all_children_named(xml_parse_string(xml), 'bar')


def test_ensure_all_children_named_multiple_success():
    xml = "<foo><bar /><bar /><bar /><baz /></foo>"
    ensure_all_children_named(xml_parse_string(xml), 'bar', 'baz')


@raises(SystemParseError)
def test_ensure_all_children_named_error():
    xml = "<foo><bar /><baz /><bar /></foo>"
    ensure_all_children_named(xml_parse_string(xml), 'bar')


def test_valid_entity_name():
    assert valid_entity_name("foo.bar")
    assert not valid_entity_name("foo/bar")
    assert not valid_entity_name("foo\\bar")
    assert not valid_entity_name("foo\\bar/baz")


def test_project_find():
    project = Project(None, search_paths=[os.path.join(_PRJ_APP_DIR, 'test_data', 'path1')])
    eg_system = project.find('example')
    assert isinstance(eg_system, System)

    project = Project(None, search_paths=[
        os.path.join(_PRJ_APP_DIR, 'test_data', 'path1', 'foo', 'bar', 'baz'),
    ])
    assert isinstance(project.find('qux'), System)

    project = Project(None, search_paths=[
        os.path.join(_PRJ_APP_DIR, 'test_data', 'path1', 'foo', 'bar'),
    ])
    assert isinstance(project.find('baz.qux'), System)

    project = Project(None, search_paths=[
        os.path.join(_PRJ_APP_DIR, 'test_data', 'path1', 'foo'),
    ])
    assert isinstance(project.find('bar.baz.qux'), System)


def test_xml_parse_file_with_includes_without_include():
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
        assert result.toxml() == result_of_xml_parse_file.toxml()
    finally:
        os.remove(prx_file.name)


def test_xml_parse_file_with_includes():
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
    assert len(new_el1s) == 1
    new_el1 = new_el1s[0]
    assert new_el1.parentNode == result_dom

    new_el2s = result_dom.getElementsByTagName('newelement2')
    assert len(new_el2s) == 1
    new_el2 = new_el2s[0]
    assert new_el2.parentNode == result_dom

    new_el3s = result_dom.getElementsByTagName('newelement3')
    assert len(new_el3s) == 1
    new_el3 = new_el3s[0]
    assert new_el3.parentNode == new_el2


def test_xml_parse_file_with_includes__absolute():
    check_xml_parse_file_with_includes__absolute_relative(absolute=True)


def test_xml_parse_file_with_includes__relative():
    check_xml_parse_file_with_includes__absolute_relative(absolute=False)


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


def test_xml_parse_file_with_includes__include_paths():
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

        assert result_dom.toxml() == expected_dom.toxml()

    finally:
        for include_dir in include_dirs:
            include_dir.cleanup()


def test_xml_parse_file_with_includes__nested():
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

    assert result_dom.toxml() == expected_dom.toxml()


@raises(SystemParseError)
def test_xml_parse_file_with_includes__include_as_root_element():
    including_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include file="{}" />"""
    included_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<include_root>
  <foo />
</include_root>"""
    check_xml_parse_file_with_includes_with_xml(including_xml, included_xml)


@raises(SystemParseError)
def test_xml_parse_file_with_includes__invalid_path():
    check_xml_parse_file_with_includes_with_xml("""<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="/123/abc/!#$" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>""")


@raises(SystemParseError)
def test_xml_parse_file_with_includes__missing_path_attribute():
    check_xml_parse_file_with_includes_with_xml("""<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>""")


@raises(SystemParseError)
def test_xml_parse_file_with_includes__child_elements():
    check_xml_parse_file_with_includes_with_xml("""<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file=".">
    <foo />
  </include>
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>""")


@raises(SystemParseError)
def test_xml_parse_file_with_includes__wrong_name_of_included_root_element():
    check_xml_parse_file_with_includes_with_xml("""<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>""", """<?xml version="1.0" encoding="UTF-8" ?><foo />""")


def test_xml_parse_file_with_includes__empty_included_root_element():
    check_xml_parse_file_with_includes_with_xml("""<?xml version="1.0" encoding="UTF-8" ?>
<system>
  <include file="{}" />
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>""", """<?xml version="1.0" encoding="UTF-8" ?><include_root />""")


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


def test_xml_include_paths():
    sys.argv[1:] = ['--prx-inc-path', 'a', '--prx-inc-path', 'b', 'gen', 'foo']
    args = get_command_line_arguments()
    assert args.prx_inc_path == ['a', 'b']

    with tempfile.TemporaryDirectory() as temp_dir:
        project_file_path = os.path.join(temp_dir, 'project.prj')
        with open(project_file_path, 'w') as file_obj:
            file_obj.write('''<?xml version="1.0" encoding="UTF-8" ?>
<project>
  <prx-include-path>1</prx-include-path>
  <prx-include-path>2</prx-include-path>
</project>''')

        project = Project(project_file_path)
        assert project._prx_include_paths == ['1', '2']  # pylint: disable=protected-access

        project = Project(project_file_path, None, args.prx_inc_path)
        assert project._prx_include_paths == ['1', '2', 'a', 'b']  # pylint: disable=protected-access


def test_check_ident():
    check_ident('foo_bar_123')
    with assert_raises(ValueError):
        check_ident('fooFbar')
    with assert_raises(ValueError):
        check_ident('Foobar')
    with assert_raises(ValueError):
        check_ident('foo_%_')
