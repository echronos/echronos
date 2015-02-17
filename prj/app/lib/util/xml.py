#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that no right, title
# or interest in or to any trade mark, service mark, logo or trade name
# of NICTA or its licensors is granted.
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
import string
import collections

import xml.dom.minidom
import xml.dom.expatbuilder
from xml.parsers.expat import ExpatError

from . import util

NOTHING = util.Singleton('nothing')


def monkey_start_element_handler(self, name, attributes):
    """This function is monkey-patched over the standard start_element_handle method.

    It adds the _line and _col attributes to the element node so that later error-checking can produce useful,
    targeted error messages.

    """
    real_start_element_handler(self, name, attributes)
    node = self.curNode
    node._line = self.getParser().CurrentLineNumber
    node._col = self.getParser().CurrentColumnNumber
real_start_element_handler = xml.dom.expatbuilder.ExpatBuilderNS.start_element_handler
xml.dom.expatbuilder.ExpatBuilderNS.start_element_handler = monkey_start_element_handler


def list_all_equal(lst):
    """Return true if all elements in the list are equal."""
    return len(set(lst)) == 1


def xml_error_str(el, msg):
    """Return an error string in the form:

    filename:lineno.colno msg

    This is used for error reporting.
    """
    return "%s:%s.%s %s" % (el.ownerDocument.path, el.ownerDocument.start_line + el._line, el._col, msg)


def xml_parse_file(filename):
    """Parse XML file `filename` and return the documentElement.

    This is a thin-wrapper for the underlying standard file parsing routine that add extra attributes to the
    DOM to enable better diagnostics via the xml_error_str function.

    Note: This returns the documentElement rather than the document itself, as this is almost always what you want.
    The underlying document can be retrieved via the ownerDocument attribute on any node.

    """
    try:
        dom = xml.dom.minidom.parse(filename)
    except ExpatError as e:
        e.path = filename
        raise e

    dom.path = filename
    dom.start_line = 0

    return dom.documentElement


def xml_parse_string(string, name='<string>', start_line=0):
    """Parse an XML string.

    Optionally a name can be provided that will be used when providing diagnosics.
    In the case where the string has been extracted from another file the start_line parameter can be used to adjust
    the line number diagnostics.

    Note: This returns the documentElement rather than the document itself, as this is almost always what you want.
    The underlying document can be retrieved via the ownerDocument attribute on any node.

    """
    try:
        dom = xml.dom.minidom.parseString(string)
    except ExpatError as e:
        e.path = name
        e.lineno += start_line
        raise e

    dom.path = name
    dom.start_line = start_line
    return dom.documentElement


def xml_parse_file_with_includes(filename, include_paths=None, output_file_path=None):
    """Parse XML file as xml_parse_file() would and resolve include elements.

    All elements with the name 'include' and the attribute 'file' are replaced with the child nodes of the root DOM
    element of the XML file referenced by the 'file' attribute.

    If `output_dir` is specified, the resulting DOM is written as XML to a file with the name contained in `filename`
    in directory `output_dir`.

    """
    dom = XmlIncludeParser(include_paths).parse(filename)
    if output_file_path is not None:
        with open(output_file_path, 'w') as f:
            f.write(dom.toprettyxml())
    return dom


class XmlIncludeParser:
    def __init__(self, include_paths=None):
        if include_paths is None:
            include_paths = []
        self._include_paths = include_paths

    def parse(self, filename):
        """Parse XML file as xml_parse_file() would and resolve include elements.

        All elements with the name 'include' and the attribute 'file' are replaced with the child nodes of the root
        DOM element of the XML file referenced by the 'file' attribute.

        """
        document_element = xml_parse_file(filename)

        if document_element.tagName == 'include':
            raise SystemParseError(xml_error_str(document_element, 'The XML root element is an include element. This \
is not supported. include elements may only appear below the root element.'))

        self.resolve_includes_below_element(document_element, os.path.dirname(filename))
        return document_element

    def resolve_includes_below_element(self, el, parent_dir):
        """Recurse children of 'el' and replace all include elements with contents of included XML files."""
        for child in el.childNodes:
            if child.nodeType == child.ELEMENT_NODE and child.tagName == 'include':
                self.resolve_include_element(child, parent_dir)
            else:
                self.resolve_includes_below_element(child, parent_dir)

    def resolve_include_element(self, el, parent_dir):
        """Replace the XML element 'el' with the child nodes of the root element of the included DOM.

        This performs all necessary consistency checks to ensure the result is again a well-formed DOM.

        After this function returns, the element 'el' is no longer part of the original DOM, it is unlinked, and must
        not be accessed or used in the context of the calling function.

        """
        if len(element_children(el)) != 0:
            raise SystemParseError(xml_error_str(el, 'Expected no child elements in include element. Correct format \
is <include file="FILENAME" />'))

        path_attribute = get_attribute(el, 'file')
        if path_attribute == NOTHING:
            raise SystemParseError(xml_error_str(el, 'Expected include element to contain "file" attribute. Correct \
format is <include file="FILENAME" />'))

        if os.path.isabs(path_attribute):
            path_to_include = path_attribute
        else:
            for include_path in [parent_dir] + self._include_paths:
                path_to_include = os.path.join(include_path, path_attribute)
                if os.path.isfile(path_to_include):
                    break

        path_to_include = os.path.normpath(path_to_include)
        if not os.path.exists(path_to_include):
            raise SystemParseError(xml_error_str(el, 'The path {} specified in the include element does not refer to \
an existing file. \
The path is considered to be {}. \
The known prx include paths are {})'.format(path_to_include,
                                            'absolute' if os.path.isabs(path_attribute) else 'relative',
                                            self._include_paths)))

        included_root_element = self.parse(path_to_include)
        if included_root_element.tagName != 'include_root':
            raise SystemParseError(xml_error_str(included_root_element, 'The XML root element in file {} is not named\
 include_root as expected. Root elements in included XML files must have this name by convention and are removed \
implicitly by the inclusion process.'))

        parent_node = el.parentNode
        for child in included_root_element.childNodes[:]:
            parent_node.insertBefore(child, el)
        parent_node.removeChild(el)
        el.unlink()


def single_text_child(el):
    """Return the text contents of an element, assuming that the element contains a single text node.

    Zero-length child nodes are also supported, for example:

    <foo> </foo> -> returns string ' '
    <foo></foo> -> returns string ''
    <foo/> -> returns string ''

    An exception is raised if these assumption are not true.

    """
    if len(el.childNodes) == 0:
        return ''

    if len(el.childNodes) != 1 or el.childNodes[0].nodeType != el.TEXT_NODE:
        error_msg = xml_error_str(el, "expect a single text node as element child.")
        raise SystemParseError(error_msg)

    return el.childNodes[0].data


def maybe_single_named_child(el, tag_name):
    """Return a child element of 'el' with a given 'tag_name'.

    An exception is raised if there are more than one child elements with the given name.
    If there are no such children None is returned.

    """
    els = [e for e in el.childNodes if e.nodeType == e.ELEMENT_NODE and e.tagName == tag_name]
    if len(els) > 1:
        raise SystemParseError(xml_error_str(el, "Too many child nodes named %s" % tag_name))
    elif len(els) == 1:
        return els[0]
    else:
        return None


def single_named_child(el, tag_name):
    """Return a child element of 'el' with a given 'tag_name'.

    An exception will be raised if there is not exactly one child element with the specified tag_name.

    """
    chld = maybe_single_named_child(el, tag_name)
    if chld is None:
        raise SystemParseError(xml_error_str(el, "Expected a single child element named %s" % tag_name))

    return chld


def get_attribute(el, attr_name, default=NOTHING):
    """Return an attribute value for a given element.

    If the optional 'default' parameter is not set a SystemParseError exception will be raised, otherwise the default
    argument will be returned.

    A special value 'NOTHING' is used to determine if the default parameter has been set or not.
    This allows 'None' to be passed as the default value.

    """
    val = el.getAttributeNode(attr_name)
    if val is None:
        if default is NOTHING:
            raise SystemParseError(xml_error_str(el, "Expected attribute name '{}'".format(attr_name)))
        else:
            return default
    return val.value


def ensure_only_whitespace_text_children(el):
    """Raise an exeception if any of the elements TEXT_NODE childNodes have non-whitespace characters."""
    def is_whitespace(c):
        return c in string.whitespace
    if any(not all(map(is_whitespace, e.data)) for e in el.childNodes if e.nodeType == e.TEXT_NODE):
        raise SystemParseError(xml_error_str(el, "Expected only non-whitespace children."))


def element_children(el, ensure_unique=False, ensure_named=None, matching=None, only_whitespace_text=False):
    """Return all element chidren.

    If `ensure_unique` is True, then a SystemParseError will be raised if the any of the child
    have the same tag name (see also: ensure_unique_tag_names).

    If `ensure_named` is not None, then a SystemParseError will be raised if the any of the child
    elements do not have the tagName `ensure_named` (see also: ensure_all_children_name).

    If `only_whitespace_text` is True then a SystemParseError will be raised if any of the child
    nodes are non-whitespace text.

    """
    children = [c for c in el.childNodes if c.nodeType == c.ELEMENT_NODE]
    if matching is not None:
        children = [c for c in children if c.tagName == matching]
    if ensure_unique:
        ensure_unique_tag_names(children)
    if ensure_named:
        ensure_all_children_named(el, ensure_named)
    if only_whitespace_text:
        ensure_only_whitespace_text_children(el)

    return children


def ensure_all_children_named(el, *names):
    """Raise an exception if any of the children elements are not in the list of 'names'."""
    if any(e.tagName not in names for e in el.childNodes if e.nodeType == e.ELEMENT_NODE):
        raise SystemParseError(xml_error_str(el, "Expected only element children named on of %s" % names))


def ensure_unique_tag_names(els):
    """Raise an exception if any tag names are duplicated."""
    seen = set()
    for el in els:
        if el.tagName in seen:
            raise SystemParseError(xml_error_str(el, "Unexpected duplicate tag name %s" % el.tagName))
        seen.add(el.tagName)


def any_element_children(el):
    """Return true if any childNodes are element nodes."""
    return len(element_children(el)) > 0


def maybe_get_element_list(el, list_name, list_item_name):
    """Given an element 'el' try and find a list of sub-elements.

    Return None if it can't be found.
    Raise an exception if the 'list_name' element contains elements that
    aren't 'list_item_name'

    E.g:
    <el>
      <list_name>
         <list_item_name />
         <list_item_name />
      </list_name>
    </el>

    Returns: [<list_item_name />, <list_item_name />]
    """
    list_el = maybe_single_named_child(el, list_name)
    if list_el is None:
        return None

    ensure_only_whitespace_text_children(list_el)
    list_els = [e for e in list_el.childNodes if e.nodeType == e.ELEMENT_NODE]
    if any([e.tagName != list_item_name for e in list_els]):
        raise SystemParseError(xml_error_str(el, "Expected only elements named %s" % list_item_name))

    return list_els


def dict_has_keys(d, *keys):
    """Return True if the dictionary has all keys."""
    for k in keys:
        if not k in d:
            return False
    return True


valid_schema_types = ['dict', 'list', 'string', 'bool', 'int', 'c_ident', 'ident', 'object']

valid_constraint_types = ['one_of']


def check_constraint_is_valid(constraint):
    """Raise SchemaInvalid exception is the constraint is not valid.


    """
    if not dict_has_keys(constraint, 'type', 'elements'):
        raise SchemaInvalidError("Constraint should have type and elements keys.")

    if constraint['type'] not in valid_constraint_types:
        raise SchemaInvalidError("Constraint type {} is not valid.".format(constraint['type']))


def check_schema_is_valid(schema, key_path=None):
    """Raise SchemaInvalidError exception if the schema is not valid.

    The None object is a valid schema.

    """
    if key_path is None:
        key_path = []
    name = None

    def error(msg):
        if name is None and len(key_path) == 0:
            key_name = None
        elif name is None:
            key_name = '.'.join(key_path)
        else:
            key_name = '.'.join(key_path + [name])
        key_msg = '' if key_name is None else 'key:{} '.format(key_name)
        raise SchemaInvalidError("Schema {}is invalid: {}".format(key_msg, msg))

    if schema is None:
        return
    if not isinstance(schema, collections.Mapping):
        error("except schema to be a mapping type.")
    if not dict_has_keys(schema, 'type', 'name'):
        error("except schema to have 'type' and 'name' fields.")
    name = schema['name']
    if schema['type'] not in valid_schema_types:
        error("type '{}' is invalid.".format(schema['type']))

    if 'default' in schema and schema['default'] is not None:
        default = schema['default']
        if schema['type'] == 'ident':
            try:
                check_ident(default)
            except ValueError as e:
                error("default value has a bad type ({})".format(e))
        if schema['type'] == 'list':
            if default != []:
                error("list default can only be an empty list.")

    if schema['type'] == 'dict':
        if not dict_has_keys(schema, 'dict_type'):
            error("when type is 'dict' except 'dict_type' to be defined.")
        if not isinstance(schema['dict_type'], tuple):
            error("'dict_type' should be a tuple")
        dict_schemas, constraints = schema['dict_type']
        for each in dict_schemas:
            check_schema_is_valid(each, key_path + [schema['name']])
        for each in constraints:
            check_constraint_is_valid(each)

    if schema['type'] == 'list':
        if not dict_has_keys(schema, 'list_type'):
            error("when type is 'list' except 'list_type' to be defined.")
        check_schema_is_valid(schema['list_type'])


def xml2schema(el):
    """Return a schema object from an XML description.

    The schema object returned by this function is suitable for passing to the xml2dict function.
    See the documentation of xml2dict for details on the schema format.

    The XML format of the schema is relatively straight forward. As an example:

    <schema>
     <entry name="taskid_size" type="int" default="8"/>
     <entry name="num_tasks" type="int"/>
     <entry name="prefix" type="c_ident" default="rtos_" />
     <entry name="tasks" type="list">
      <entry name="task" type="dict">
       <entry name="idx" type="int" />
       <entry name="entry" type="c_ident" />
       <entry name="name" type="ident" />
       <entry name="stack_size" type="int" />
      </entry>
     </entry>
    </schema>

    The `name`, `type` and `default` attributes map directly to the same named field within the schema object.
    If `type` is list, there should be a single sub-element describing the list element.
    All list element must by the same.
    If the `type` is dict, there should be more one or more sub-element describing the valid dictionary entries.

    """
    def read_constraint(el):
        constraint = {
            'type': get_attribute(el, 'type'),
            'elements': list(map(single_text_child,
                                 element_children(el, matching='entry', only_whitespace_text=True)))
        }
        if constraint['type'] not in valid_constraint_types:
            err_str = xml_error_str(el, "Invalid constraint type '{}'".format(constraint['type']))
            raise SystemParseError(err_str)

        return constraint

    def read_entry(el):
        entry = {
            'name': get_attribute(el, 'name'),
            'type': get_attribute(el, 'type', 'string'),
            'default': get_attribute(el, 'default', None),
        }

        try:
            entry['optional'] = {'true': True, 'false': False}[get_attribute(el, 'optional', 'false').lower()]
        except KeyError:
            raise SystemParseError(xml_error_str(el, "Attribute 'optional' should be 'true' or 'false'."))

        _type = entry['type']
        if _type not in valid_schema_types:
            err_str = xml_error_str(el, "Invalid type '{}' should be one of {}".format(_type, valid_schema_types))
            raise SystemParseError(err_str)

        if _type == 'list':
            entry['list_type'] = read_entry(single_named_child(el, 'entry'))
            entry['auto_index_field'] = get_attribute(el, 'auto_index_field', None)
            if entry['default'] is not None:
                if entry['default'] != '[]':
                    msg = "Invalid default '{}'. Only empty list '[]' is supported as a list default."
                    raise SystemParseError(xml_error_str(el, msg.format(entry['default'])))
                entry['default'] = util.LengthList([])
                entry['optional'] = True
        elif _type == 'dict':
            entry['dict_type'] = read_dict_schema(el)
        elif _type == 'object':
            entry['object_group'] = get_attribute(el, 'group', None)

        return entry

    def read_dict_schema(el):
        ensure_all_children_named(el, 'entry', 'constraint')
        fields = list(map(read_entry, element_children(el, matching='entry', only_whitespace_text=True)))
        constraint_els = element_children(el, matching='constraint', only_whitespace_text=True)
        constraints = list(map(read_constraint, constraint_els))
        return (fields, constraints)

    return {
        'name': 'module',
        'type': 'dict',
        'dict_type': read_dict_schema(el)
    }


def check_ident(s):
    """Check that a string (`s`) is a valid `ident`.

    Raise a ValueError if `s` is not valid.

    A valid ident should contains lower-case ascii [a-z], digits [0-9] and '_'.
    The first character should be a lower-case ascii. [a-z]

    """
    if len(s) == 0:
        raise ValueError("Ident string must not be empty.")

    if s[0] not in string.ascii_lowercase:
        raise ValueError("First character of an indent must be a lower-case ascii character.")

    valid_chars = string.ascii_lowercase + string.digits + '_'
    if any(c not in valid_chars for c in s):
        raise ValueError("Ident must only contains ASCII lower-case, digits and '_'")


def xml2dict(el, schema=None):
    """Given a well-formed XML DOM element, return a Python dictionary indexed by XML tag name.
    E.g:

    <X>
      <foo>1</foo>
      <bar>2</bar>
      <baz>2</baz>
    </X>

    results in {'foo': '1', 'bar': '2', 'baz': '2'}

    Element contents can either be strings, or other elements (not a mixture of the two.)
    When element contents are elements, they are converted to either a python list, or dictionary. E.g:

    <X>
     <foo>
      <bar>1</bar>
      <bar>2</bar>
      <bar>3</bar>
     </foo>
    </X>

    results in {'foo': ['1', '2', '3']}

    However,

    <X>
     <foo>
      <a>1</a>
      <b>2</b>
      <c>3</c>
     </foo>
    </X>

    results in {'foo': {'a': '1', 'b': '2', 'c': '3'}}

    Note: There is currently an unfortunate ambiguity in the case where an element has a single child.
    It is not clear if this should be returned as a single entry list, or a single entry dictionary.
    Currently this has not been problematic in practise, however users should be aware.

    Note: A second ambiguity occurs when an element is empty.
    This could represent an empty list, and empty dict, or a zero-length string.
    Currently it is resolved as a zero-length string.

    A schema object can be provided to ensure the input is well-formed, and additionally resolve
    the potential ambiguities described above.
    The schema object is a recursive python dictionary with the following fields:

      name: the name of the tag.
      type: describes the type: ['int', 'string', 'c_defn', 'dict', 'list']
      default: default value (not used for 'dict' or 'list').
      dict_type: a tuple containing -
          1: A list of of schema objects which describes the form of the dictionary.
          2: A list of constraints
      list_type: a single schema object which describes the form of list elements.

    """
    check_schema_is_valid(schema)

    class ObjectProxy:
        """An ObjectProxy stands in for a real object.

        This proxied objects are resolved once a first-pass has been completed and all names are known.

        """
        def __init__(self, name, group, el):
            self.name = name
            self.group = group
            self.el = el

    def resolve_proxies(dct):
        # Find all ObjectProxies and resolve them.
        for key, val in ((k, v) for k, v in util.config_traverse(dct) if isinstance(v, ObjectProxy)):
            try:
                real_val = util.list_search(dct[val.group], 'name', val.name)
            except KeyError:
                raise SystemParseError(xml_error_str(val.el, "Can't find object named '{}'".format(val.name)))
            util.config_set(dct, key, real_val)

    def get_dict_val(el, dict_type):
        if dict_type is not None:
            schema, constraints = dict_type
        else:
            schema, constraints = None, []
        children = element_children(el, ensure_unique=True)
        if not schema:
            return {c.tagName: get_el_val(c, None, el) for c in children}

        r = {}
        els = asdict(children, attr='tagName')
        for entry in schema:
            name = entry['name']
            r[name] = get_el_val(els.get(name), entry, el)
            if name in els:
                del els[name]

        if len(els):
            fst = next(iter(els.values()))
            raise SystemParseError(xml_error_str(fst, "Unexpected configuration entry '{}'".format(fst.tagName)))

        # Check constraints
        for constraint in constraints:
            # Implementation note: if/when the number of constraint types increases
            # this may be better handled by a jump-table or other polymorphic approach.
            if constraint['type'] == 'one_of':
                msg = "Expected exactly one of the following elements: {}"
                err_str = xml_error_str(el, msg.format(constraint['elements']))
                if len([True for elem in constraint['elements'] if r[elem] is None]) != 1:
                    raise SystemParseError(err_str)

        return r

    def get_type(el, schema):
        if schema:
            return schema['type']
        else:
            if any_element_children(el):
                if list_all_equal([c.tagName for c in element_children(el)]):
                    return 'list'
                else:
                    return 'dict'
            else:
                return 'string'

    def get_text_value(el, schema, parent):
        if el is not None:
            return single_text_child(el)
        else:
            if schema.get('default') is not None:
                return schema['default']
            elif schema.get('optional', False):
                return None
            else:
                msg = xml_error_str(parent, "Required config field '{}' missing.".format(schema['name']))
                raise SystemParseError(msg)

    def get_el_val(el, schema, parent):
        """Return a Python object value for the given element."""
        assert schema is not None or el is not None

        _type = get_type(el, schema)

        if schema is not None and el is not None:
            if el.tagName != schema['name']:
                raise SystemParseError(
                    xml_error_str(el, "Expected tagName: {}, not {}".format(schema['name'], el.tagName)))

        if _type == 'dict':
            if el is not None:
                return get_dict_val(el, schema['dict_type'] if schema else None)
            else:
                return {}
        elif _type == 'list':
            if el is not None:
                r = [get_el_val(c, schema['list_type'] if schema else None, el) for c in element_children(el)]
            else:
                if schema.get('default') is not None:
                    return schema['default']
                elif schema.get('optional', False):
                    return None
                else:
                    msg = xml_error_str(parent, "Required config field '{}' missing.".format(schema['name']))
                    raise SystemParseError(msg)

            auto_index_field = schema.get('auto_index_field') if schema is not None else None
            if auto_index_field is not None:
                try:
                    util.add_index(r, auto_index_field)
                except ValueError as e:
                    raise SystemParseError(xml_error_str(el, str(e)))
            return util.LengthList(r)

        # If it isn't a compound type, get the value
        val = get_text_value(el, schema, parent)

        # Optional values may be None; return immediately.
        if val is None:
            return val

        # and then do type checking an coercion
        if _type == 'string':
            return val
        elif _type == 'bool':
            try:
                return {'true': True, 'false': False}[val.lower()]
            except KeyError:
                raise SystemParseError(xml_error_str(el, "Error converting '{}' to boolean.".format(val)))
        elif _type == 'int':
            try:
                return int(val, base=0)
            except ValueError as e:
                raise SystemParseError(xml_error_str(el, "Error converting '{}' to integer: {}".format(val, e)))
        elif _type == 'c_ident':
            # Check this is really a C identifier
            return val
        elif _type == 'ident':
            try:
                check_ident(val)
            except ValueError as e:
                raise SystemParseError(xml_error_str(el, "Error parsing ident '{}'. {}".format(val, e)))
            return val
        elif _type == 'object':
            return ObjectProxy(val, schema['object_group'], el)
        else:
            assert False

    dct = get_el_val(el, schema, None)
    resolve_proxies(dct)
    return dct


def asdict(lst, key=None, attr=None):
    """Convert a list of dictionaries or objects in to a dictionary indexed by either a key or attribute.

    E.g:
    > asdict([{'foo': 1}, {'foo': 2}], key='foo')
    {1: {'foo': 1}, 2: {'foo': 2}}

    """
    if key is not None and attr is not None:
        raise Exception("Only key or attr should be set (not both)")

    if attr is not None:
        lookup = lambda x: getattr(x, attr)
    elif key is not None:
        lookup = lambda x: x[key]
    else:
        raise Exception("Key or attr should be set")

    return {lookup(x): x for x in lst}


class SchemaInvalidError(Exception):
    """Raised by `check_schema_is_valid` if a schema is invalid."""


class UserError(Exception):
    """This is the base class for a set of exceptions that will be reported to the user.

    These errors are raised due to an user input of some type and are ultimately display
    to the end-user of the tool.

    """


class SystemParseError(UserError):
    """Raised when parsing system definition files."""
