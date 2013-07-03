from prj import *
from nose.tools import assert_raises, raises


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


def test_paths_overlap():
    assert paths_overlap(['foo', 'bar']) == (False, None)
    assert paths_overlap(['foo', 'foo/bar']) == (True, ('foo', 'foo/bar'))
