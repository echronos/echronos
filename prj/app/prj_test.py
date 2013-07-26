import os
import tempfile
from prj import *
from nose.tools import assert_raises, raises

base_dir = os.path.dirname(__file__)


def test_dict_has_keys():
    d = {'foo': 37, 'bar': 25}
    assert dict_has_keys(d, 'foo')
    assert not dict_has_keys(d, 'baz')
    assert dict_has_keys(d, 'foo', 'bar')


def test_schema_is_valid():
    check_schema_is_valid(None)
    good = {
        'type': 'dict',
        'name': 'module',
        'dict_type':
        [
            {'type': 'string', 'name': 'target', 'default': ''}
        ]
    }
    check_schema_is_valid(good)

    no_name = {
        'type': 'dict',
        'dict_type':
        [
            {'type': 'string', 'name': 'target', 'default': ''}
        ]
    }
    assert_raises(SchemaInvalid, check_schema_is_valid, no_name)

    bad_dict_type = {
        'name': 'module',
        'type': 'dict',
        'dict_type':
        {
            'foo': {'type': 'string', 'name': 'target', 'default': ''}
        }
    }
    assert_raises(SchemaInvalid, check_schema_is_valid, bad_dict_type)

    invalid_nested = {
        'type': 'dict',
        'name': 'module',
        'dict_type':
        [
            {'name': 'target', 'default': ''}
        ]
    }
    assert_raises(SchemaInvalid, check_schema_is_valid, invalid_nested)


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


def test_asdict_key():
    x = {'foo': 1}
    y = {'foo': 2}
    assert asdict([x, y], key='foo') == {1: x, 2: y}


def test_asdict_attr():
    class Simple(object):
        def __init__(self, foo):
            self.foo = foo

        def __repr__(self):
            return "<Simple: {}>".format(self.foo)

    x = Simple(1)
    y = Simple(2)

    assert asdict([x, y], attr='foo') == {1: x, 2: y}


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
    except ExpatError as e:
        assert e.path == 'mal'
        assert e.lineno == 5


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
    p = Project(None, search_paths=[os.path.join(base_dir, 'test_data', 'path1')])
    eg_system = p.find('example')
    assert isinstance(eg_system, System)

    qux = os.path.join(base_dir, 'test_data', 'path1', 'foo', 'bar', 'baz', 'qux.prx')
    p = Project(None, search_paths=[
        os.path.join(base_dir, 'test_data', 'path1', 'foo', 'bar', 'baz'),
    ])
    assert isinstance(p.find('qux'), System)

    p = Project(None, search_paths=[
        os.path.join(base_dir, 'test_data', 'path1', 'foo', 'bar'),
    ])
    assert isinstance(p.find('baz.qux'), System)

    p = Project(None, search_paths=[
        os.path.join(base_dir, 'test_data', 'path1', 'foo'),
    ])
    assert isinstance(p.find('bar.baz.qux'), System)


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
        result_of_xml_parse_file_with_includes = xml_parse_file_with_includes(prx_file.name)
        result_of_xml_parse_file = xml_parse_file(prx_file.name)
        assert result_of_xml_parse_file_with_includes.toxml() == result_of_xml_parse_file.toxml()
    finally:
        os.remove(prx_file.name)


def test_xml_parse_file_with_includes():
    include_xml = """<?xml version="1.0" encoding="UTF-8" ?>
<newelement />"""
    include_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
    include_file.write(include_xml)
    include_file.close()
    prx_template = """<?xml version="1.0" encoding="UTF-8" ?>
<system>{}
  <modules>
    <module name="foo">
      <bar>baz</bar>
    </module>
  </modules>
</system>"""
    prx_with_include_xml = prx_template.format('<include file="{}" />'.format(os.path.basename(include_file.name)))
    prx_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
    prx_file.write(prx_with_include_xml)
    prx_file.close()
    prx_without_include_xml = prx_template.format('')
    try:
        result_of_xml_parse_file_with_includes = xml_parse_file_with_includes(prx_file.name)
        expected_result = xml_parse_string(prx_without_include_xml)
        expected_result.insertBefore(expected_result.ownerDocument.createElement('newelement'),
                                     expected_result.firstChild)
        assert result_of_xml_parse_file_with_includes.toxml() == expected_result.toxml()
    finally:
        os.remove(prx_file.name)
        os.remove(include_file.name)


@raises(SystemParseError)
def test_xml_parse_file_with_includes_invalid_path():
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
def test_xml_parse_file_with_includes_missing_path_attribute():
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
def test_xml_parse_file_with_includes_missing_path_child_elements():
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


def check_xml_parse_file_with_includes_with_xml(xml):
    prx_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
    prx_file.write(xml)
    prx_file.close()
    try:
        return xml_parse_file_with_includes(prx_file.name)
    finally:
        os.remove(prx_file.name)
