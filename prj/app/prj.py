#!/usr/bin/env python3.3
"""Firmware system project management tool.

Main entry point.

"""
if __name__ == "__main__":
    import sys
    import os

    # When prj is frozen, there is no need to play games with the path.
    frozen = __file__ == "<frozen>"
    if not frozen:
        for pth in ['pystache', 'ply', 'lib']:
            sys.path.insert(0, os.path.join(os.path.dirname(__file__), pth))
else:
    # The prj module is only imported as a module for testing purposes.
    # In this case it definitely isn't frozen.
    # FIXME: It likely makes sense to refactor prj.py so that this top level file is purely focused on script
    # execution and most logic is placed in a separate modules.
    frozen = False

# Logging is set up first since this is critical to the rest of the application working correctly.
# It is possible that other modules will perform logging during import, so make sure this is very early.
import logging as _logging  # Avoid unintended using of 'logging'
# By default log everything from INFO up.
logger = _logging.getLogger('prj')
logger.setLevel(_logging.INFO)
_logging.basicConfig()


# Ensure that basicConfig is only called once.
def error_fn(*args, **kwargs):
    raise Exception("basicConfig called multiple times.")
_logging.basicConfig = error_fn

import xml.dom.minidom
from xml.parsers.expat import ExpatError
import argparse
import collections
import functools
import glob
import imp
import os
import pdb
import pystache.parser
import pystache.renderer
import re
import shutil
import signal
import string
import subprocess
import sys
import traceback
import util.util
import xml.dom.expatbuilder


# Configure the pystache module
pystache.defaults.MISSING_TAGS = 'strict'

# Place this module in to the modules namespace as 'prj' which enables
# any plug-in modules to "import prj". This may be done differently
# in the future as some of this script is split in to different
# files.
sys.modules['prj'] = sys.modules[__name__]


NOTHING = util.util.Singleton('nothing')
SIG_NAMES = dict((k, v) for v, k in signal.__dict__.items() if v.startswith('SIG'))


def canonical_path(path):
    return os.path.normcase(os.path.normpath(os.path.realpath(os.path.abspath(path))))


def canonical_paths(paths):
    return [canonical_path(path) for path in paths]


def follow_link(l):
    """Return the underlying file form a symbolic link.

    This will (recursively) follow links until a non-symbolic link is found.
    No cycle checks are performed by this function.

    """
    if not os.path.islink(l):
        return l
    p = os.readlink(l)
    if os.path.isabs(p):
        return p
    return follow_link(os.path.join(os.path.dirname(l), p))


def show_exit(exit_code):
    sig_num = exit_code & 0xff
    exit_status = exit_code >> 8
    if sig_num == 0:
        return "exit: {}".format(exit_status)
    else:
        return "signal: {}".format(SIG_NAMES.get(sig_num, 'Unknown signal {}'.format(sig_num)))


def cls_name(cls):
    """Return the full name of a class (including the it's module name)."""
    return "{}.{}".format(cls.__module__, cls.__name__)


def pystache_render(file_in, file_out, config):
    """Render a pystache template. file_in is the input
    file path. file_out is the output file path. config is a
    dictionary with the context data.

    Note: this function will create any directories neccessary to
    enable writing of the output file.

    """
    renderer = pystache.renderer.Renderer()
    renderer.register('u', lambda x: x.upper())

    with open(file_in, 'r') as inp:
        template_data = inp.read()

    parsed_template = pystache.parser.parse(template_data, name=file_in)

    try:
        data = renderer.render(parsed_template, config)
    except pystache.common.PystacheError as e:
        raise SystemBuildError("Error rendering template '{}'. {}.".format(e.location, str(e)))

    os.makedirs(os.path.dirname(file_out), exist_ok=True)

    with open(file_out, 'w') as outp:
        outp.write(data)


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

# We don't want byte-code written to disk for any of the plug-ins that we load,
# so disable it here. It would be nicer to do this on a per-plugin basis but
# that doesn't appear possible from the Python source
sys.dont_write_bytecode = True


# Generic helper functions
def list_all_equal(lst):
    """Return true if all elements in the list are equal."""
    return len(set(lst)) == 1


# XML helper functions
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
            raise SystemParseError(xml_error_str(el, "Expected attribute name {}".format(attr_name)))
        else:
            return default
    return val.value


def ensure_only_whitespace_text_children(el):
    """Raise an exeception if any of the elements TEXT_NODE childNodes have non-whitespace characters."""
    def is_whitespace(c):
        return c in string.whitespace
    if any(not all(map(is_whitespace, e.data)) for e in el.childNodes if e.nodeType == e.TEXT_NODE):
        raise SystemParseError(xml_error_str(el, "Expected only non-whitespace children."))


def element_children(el, ensure_unique=False, ensure_named=None, only_whitespace_text=False):
    """Return all element chidren.

    If `ensure_unique` is True, then a SystemParseError will be raised if the any of the child
    have the same tag name (see also: ensure_unique_tag_names).

    If `ensure_named` is not None, then a SystemParseError will be raised if the any of the child
    elements do not have the tagName `ensure_named` (see also: ensure_all_children_name).

    If `only_whitespace_text` is True then a SystemParseError will be raised if any of the child
    nodes are non-whitespace text.

    """
    children = [c for c in el.childNodes if c.nodeType == c.ELEMENT_NODE]
    if ensure_unique:
        ensure_unique_tag_names(children)
    if ensure_named:
        ensure_all_children_named(el, ensure_named)
    if only_whitespace_text:
        ensure_only_whitespace_text_children(el)

    return children


def ensure_all_children_named(el, name):
    """Raise an exception if any of the children elements are not named 'name'."""
    if any(not e.tagName == name for e in el.childNodes if e.nodeType == e.ELEMENT_NODE):
        raise SystemParseError(xml_error_str(el, "Expected only element children named %s" % name))


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


valid_schema_types = ['dict', 'list', 'string', 'int', 'c_ident']


def check_schema_is_valid(schema, key_path=None):
    """Raise SchemaInvalid exception if the schema is not valid.

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
        raise SchemaInvalid("Schema {}is invalid: {}".format(key_msg, msg))

    if schema is None:
        return
    if not isinstance(schema, collections.Mapping):
        error("except schema to be a mapping type.")
    if not dict_has_keys(schema, 'type', 'name'):
        error("except schema to have 'type' and 'name' fields.")
    name = schema['name']
    if schema['type'] not in valid_schema_types:
        error("type '{}' is invalid.".format(schema['type']))

    if schema['type'] == 'dict':
        if not dict_has_keys(schema, 'dict_type'):
            error("when type is 'dict' except 'dict_type' to be defined.")
        if not isinstance(schema['dict_type'], collections.Sequence):
            error("'dict_type' should be a sequence")
        for each in schema['dict_type']:
            check_schema_is_valid(each, key_path + [schema['name']])


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
       <entry name="name" type="c_ident" />
       <entry name="stack_size" type="int" />
      </entry>
     </entry>
    </schema>

    The `name`, `type` and `default` attributes map directly to the same named field within the schema object.
    If `type` is list, there should be a single sub-element describing the list element.
    All list element must by the same.
    If the `type` is dict, there should be more one or more sub-element describing the valid dictionary entries.

    """
    def read_entry(el):
        entry = {
            'name': get_attribute(el, 'name'),
            'type': get_attribute(el, 'type', 'string'),
            'default': get_attribute(el, 'default', None),
        }

        _type = entry['type']
        if _type not in valid_schema_types:
            err_str = xml_error_str(el, "Invalid type '{}' should be one of {}".format(_type, valid_schema_types))
            raise SystemParseError(err_str)

        if _type == 'list':
            entry['list_type'] = read_entry(single_named_child(el, 'entry'))
        elif _type == 'dict':
            entry['dict_type'] = read_dict_schema(el)

        return entry

    def read_dict_schema(el):
        return list(map(read_entry, element_children(el, ensure_named='entry', only_whitespace_text=True)))

    return {
        'name': 'module',
        'type': 'dict',
        'dict_type': read_dict_schema(el)
    }


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
      dict_type: a dict of schema object (index by name) which describes the form of the dictionary.
      list_type: a single schema object which describes the form of list elements.

    """
    check_schema_is_valid(schema)

    def get_dict_val(el, schema):
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
            raise SystemParseError(xml_error_str(fst, "Unexpected entry: {}".format(fst.tagName)))

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
            if schema['default'] is not None:
                return schema['default']
            else:
                raise SystemParseError(xml_error_str(parent, "Required config {} missing.".format(schema['name'])))

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
                return [get_el_val(c, schema['list_type'] if schema else None, el) for c in element_children(el)]
            else:
                return []

        # If it isn't a compound type, get the value
        val = get_text_value(el, schema, parent)

        # and then do type checking an coercion
        if _type == 'string':
            return val
        elif _type == 'int':
            try:
                return int(val, base=0)
            except Exception as e:
                raise SystemParseError(xml_error_str(el, "Error converting '{}' to integer: {}".format(val, e)))
        elif _type == 'c_ident':
            # Check this is really a C identifier
            return val
        else:
            assert False

    return get_el_val(el, schema, None)


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


class SchemaInvalid(Exception):
    """Raised by `check_schema_is_valid` if a schema is invalid."""


class ResourceNotFoundError(Exception):
    """Raised when the system is unable to find a file or other resource.

    This is very similar to the builtin FileNotFoundError however, this
    allows us customisation of the error message.
    """


class SystemParseError(Exception):
    """Raised when parsing system definition files."""


class SystemConsistencyError(Exception):
    """Indicates that the system, as instantiated from its system definition, is internally inconsistent.
    For example, a required module may be missing.

    """


class SystemBuildError(Exception):
    """Raised when an error occurs during system build."""


class SystemLoadError(Exception):
    """Raised when an error occurs while loading and starting a system image on a device."""


class EntityLoadError(Exception):
    """Raise when unable to resolve a entity reference."""


class EntityNotFound(Exception):
    """Raised when an entity can not be found."""


class ProjectError(Exception):
    """Raised when there is an error constructing the Project class."""


class ProjectStartupError(Exception):
    """Raised when there is an error initialising the start-script."""


def valid_entity_name(name):
    """Return true if the name is a valid entity name.

    Entity names can not contain / or \ characters.

    """
    return not any([bad_char in name for bad_char in '/\\'])


def execute(args, env=None):
    """Execute a command. This wraps sucprocess.call in appropriate logging
    calls to display the command being executed, and raising an exception in
    the case of an error."""
    cmd_line = ' '.join(args)
    logger.info('Executing: %s' % cmd_line)
    try:
        code = subprocess.call(args, env=env)
    except FileNotFoundError as exc:
        raise SystemBuildError("Command {} raise exception: {}".format(cmd_line, exc))
    if code != 0:
        raise SystemBuildError("Command {} returned non-zero error code: {}".format(cmd_line, code))


class Header:
    """Header is a very simple container class that keeps track of an XML element
    that is associated with a header file name.

    This association is valuable as it enables better error reporting.

    """
    def __init__(self, path, code_gen, xml_element):
        self.path = path
        self.code_gen = code_gen
        self.xml_element = xml_element


class ModuleInstance:
    """A module instance dissociates a module object from the system it is instantiated in and the related
    configuration information.

    """

    def __init__(self, module, system, config):
        """Create a new `ModuleInstance` of the specified `module` with config data `config`.

        The `ModuleInstance` is created with-in the context of the specified `system`.

        """
        self._config = config
        self._module = module
        self._system = system

    def __repr__(self):
        return '<{} {} of module {}>'.format(self.__class__.__name__, id(self), repr(self._module))

    def __getattr__(self, name):
        """ModuleInstances do not have methods, however they provide shortcuts to the underlying Module's methods.

        `instance.<foo>()` is a short-cut for `instance._module.<foo>(instance._system, instance._config)`

        """
        try:
            func = getattr(self._module, name)
        except AttributeError:
            raise AttributeError("ModuleInstance '{}' has no attribute '{}'".format(self, name))

        if not callable(func):
            raise AttributeError("ModuleInstance '{}' has no attribute '{}'".format(self, name))

        @functools.wraps(func)
        def _wrap(*args, **kwargs):
            return func(self._system, self._config, *args, **kwargs)

        return _wrap


class Module:
    """Represents a component of a system, as systems are containers for modules and defined by the specific set of
    modules they contain.

    Subclasses of this class implement specific types of modules and their properties.
    This includes, for example, subclasses representing source files or executable actions.

    A module can exist multiple times within a system (or the project as a whole).
    Each time a module in included within a system it is configured.
    The module's `configure` method takes in the user's configuration data (as an XML element) and returns an
    object containing the configuration data.
    Often this will be a dictionary, however the exact types is module specific.
    This configuration data will be later be passed explicitly to the `validate` and `prepare` methods.

    """
    schema = NOTHING

    def configure(self, xml_config):
        """Configure a module.

        `xml_config` is an XML element describing the configuration of the module.
        The method should return an object, which will later be passed to `prepare` and `validate`.
        This method can raise exceptions if the `xml_config` is invalid.

        """
        if self.schema is NOTHING:
            return None
        else:
            return xml2dict(xml_config, self.schema)

    def validate(self, system, config):
        """Validate that the `config` is correct within the context of the given `system`.

        This would include such things as dependencies on other modules.
        This is called after all the system's module instances are created, so as to allow for the possibility of
        mutual dependencies or other advanced checks.

        """
        pass

    def prepare(self, system, config, **kwargs):
        """Prepare the `system` for building based on a specific module `config`.

        This method should be implemented in a sub-class. It should update the system object
        making it ready to be passed to a Builder module. Additionally it may update the
        filesystem to generate files.

        """
        pass

    def post_prepare(self, system, config):
        pass

    def __repr__(self):
        return '<{}>'.format(cls_name(self.__class__))


class NamedModule(Module):
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return '<{} name="{}">'.format(cls_name(self.__class__), self.name)


class SourceModule(NamedModule):
    """A Module contains the implementation of some aspect of the system.

    See the user manual for more details.
    Currently only minimal modules, which are raw .c files are supported.

    """
    def __init__(self, name, filename):
        """Create a new `Module` with a given `name`.

        The `filename` currently must point to a .c file, but this will be expanded in time.
        `project` is a pointer back to the project in which it exists.

        """
        super().__init__(name)
        self.filename = filename
        self.code_gen = None
        self.headers = []
        self.schema = None  # schema defaults to none

        # Determine what type of Module this is.
        if filename.endswith('.c'):
            # This is a very non-perfect comment extractor
            state = "find_open"
            content = []
            start_lineno = 0
            for lineno, line in enumerate(open(filename).readlines()):
                if state == "find_open" and line.strip() == '/*<module>':
                    start_lineno = lineno
                    content.append('<module>\n')
                    state = "find_close"
                elif state == "find_close":
                    if line.strip() == "</module>*/":
                        content.append('</module>')
                        break
                    else:
                        content.append(line)
            else:
                if state == "find_close":
                    raise SystemParseError("Didn't find closing module tag")

            if content:
                self._configure_from_xml(xml_parse_string(''.join(content), filename, start_lineno))
            else:
                self.module_type = 'default'
                # If the module doesn't have something specified explicitly then if there is
                # <module>.h, then use that.
                possible_header = filename[:-1] + 'h'
                if os.path.exists(possible_header):
                    self.headers.append(Header(possible_header, None, None))

        elif filename.endswith('.s') or filename.endswith('.asm'):
            self.module_type = 'default'
        else:
            raise SystemParseError("Module %s[%s] has invalid filename" % (name, filename))

    def _configure_from_xml(self, dom):
        """Configure a module based on XML input. This may be XML that was
        extracted from a marked-up comment, or directly in an XML file.

        """
        cg_el = maybe_single_named_child(dom, 'code_gen')
        if cg_el:
            code_gen = single_text_child(cg_el)
            if code_gen not in ['template']:
                raise SystemParseError(xml_error_str(cg_el, "not a supported code_gen type: %s" % code_gen))
            self.code_gen = code_gen

        # Get all headers
        hdr_els = maybe_get_element_list(dom, "headers", "header")
        if hdr_els is None:
            hdr_els = []

        def fix_path(hdr):
            if os.path.isabs(hdr):
                return hdr
            else:
                return os.path.join(os.path.dirname(self.filename), hdr)

        for hdr_el in hdr_els:
            code_gen = hdr_el.getAttributeNode('code_gen')
            code_gen = code_gen.value if code_gen is not None else None
            if code_gen not in [None, 'template']:
                raise SystemParseError(xml_error_str(hdr_el, "not a supported code_gen type: %s" % code_gen))
            self.headers.append(Header(fix_path(get_attribute(hdr_el, "path")), code_gen, hdr_el))

        schema = maybe_single_named_child(dom, 'schema')
        self.schema = xml2schema(schema) if schema else None

    def prepare(self, system, config, *, copy_all_files=False):
        """prepare the `system` for building based on the specific config.

        This includes copying any header files to the correct locations, and running any templating.
        All modules are only prepared after validation of all modules has occurred.
        Compilation does not occur until all modules are prepared.

        This is currently only stubbed.

        """
        if self.code_gen is None:
            if copy_all_files:
                path = os.path.join(system.output, os.path.basename(self.filename))
                shutil.copy(self.filename, path)
                logger.info("Preparing: copy %s -> %s", self.filename, path)
                system.add_file(path)
            else:
                system.add_file(self.filename)

        elif self.code_gen == 'template':
            # Create implementation file.
            path = os.path.join(system.output, '%s.c' % os.path.basename(self.name))
            logger.info("Preparing: template %s -> %s (%s)", self.filename, path, config)
            pystache_render(self.filename, path, config)
            system.add_file(path)

        # Copy any headers across. This should use templating if that is configured.
        for header in self.headers:
            path = os.path.join(system.output, os.path.basename(header.path))
            try:
                if header.code_gen is None:
                        shutil.copy(header.path, path)
                elif header.code_gen == 'template':
                    logger.info("Preparing: template %s -> %s (%s)", header.path, path, config)
                    pystache_render(header.path, path, config)
            except FileNotFoundError as e:
                s = xml_error_str(header.xml_element, "Resource not found: {}".format(header.path))
                raise ResourceNotFoundError(s)


class Action(NamedModule):
    def __init__(self, name, py_module):
        super().__init__(name)
        assert hasattr(py_module, 'run')
        self._py_module = py_module
        if hasattr(py_module, 'schema'):
            try:
                check_schema_is_valid(py_module.schema)
            except SchemaInvalid as e:
                raise SystemLoadError("The schema declared in module '{}' is invalid. {:s}".format(name, e))
            self.schema = py_module.schema

    def run(self, system, config):
        try:
            self._py_module.run(system, config)
        except (SystemBuildError, ):
            raise
        except Exception as e:
            file_name = '{}.errors.log'.format(self.name)
            with open(file_name, "w") as f:
                traceback.print_exception(*sys.exc_info(), file=f)
            msg_fmt = "An unexpected error occured while running custom module '{}'. Traceback saved to '{}'."
            raise SystemBuildError(msg_fmt.format(self.name, file_name))


class Builder(Action):
    pass


class Loader(Action):
    pass


class System:
    def __init__(self, name, dom, project):
        """Create a new System named `name`. The system definition will be parsed from
        the `dom`, which should be an XML document object.

        """
        self.name = name
        self.dom = dom
        self.project = project
        self._c_files = []
        self._asm_files = []
        self._linker_script = None
        self._output = None
        self.__instances = None

    @property
    def linker_script(self):
        if self._linker_script is None:
            raise SystemBuildError("Linker script undefined.")
        return self._linker_script

    @linker_script.setter
    def linker_script(self, value):
        self._linker_script = value

    @property
    def output(self):
        if self._output is None:
            self._output = os.path.join(self.project.output, *self.name.split('.'))

        return self._output

    @output.setter
    def output(self, value):
        self._output = value

    @property
    def include_paths(self):
        return [self.output]

    @property
    def c_files(self):
        return self._c_files

    @property
    def asm_files(self):
        return self._asm_files

    @property
    def output_file(self):
        return os.path.join(self.output, 'system')

    def add_asm_file(self, asm_file):
        self._asm_files.append(asm_file)

    def add_c_file(self, c_file):
        self._c_files.append(c_file)

    def add_file(self, path):
        """Depending on its type, add a file to a system as one of the
        system properties, such as c_files or asm_files."""
        add_functions = {
            '.c': self.add_c_file,
            '.s': self.add_asm_file,
            '.asm': self.add_asm_file,
        }
        extension = os.path.splitext(path)[1]
        add_functions[extension](path)

    @property
    def image(self):
        """The image of this system once built.
        The system image is currently represented as the path to the linked executable.
        In the future, more complex, class-based representations of system images might be introduced.


        """
        return os.path.join(self.output, 'system')

    @property
    def _instances(self):
        if not self.__instances:
            self.__instances = self._get_instances()
        return self.__instances

    def _get_instances(self):
        """Instantiate all modules referenced in the system definition file of this system and validate them.
        This is a prerequisite to invoking such operations as build or load on a system.

        Returns a list of instances of class ModuleInstance.

        """
        # Parse the DOM to load all the entities.
        module_el = single_named_child(self.dom, 'modules')
        module_els = [e for e in module_el.childNodes if e.nodeType == e.ELEMENT_NODE and e.tagName == 'module']

        instances = []
        for m_el in module_els:
            # First find the module
            name = get_attribute(m_el, 'name')
            if not valid_entity_name(name):
                raise EntityLoadError(xml_error_str(m_el, "Invalid module name '{}'".format(name)))
            try:
                module = self.project.find(name)
            except EntityLoadError as e:
                logger.error(e)
                raise EntityLoadError(xml_error_str(m_el, 'Error loading module {}'.format(name)))

            if isinstance(module, Module):
                instance = ModuleInstance(module, self, module.configure(m_el))
                instances.append(instance)
            else:
                raise EntityLoadError(xml_error_str(m_el, 'Entity {} has unexpected type {} and cannot be \
                instantiated'.format(name, type(module))))

        os.makedirs(self.output, exist_ok=True)

        for i in instances:
            i.validate()

        return instances

    def generate(self, *, copy_all_files):
        """Generate the source for the system.

        Raise an appropriate exception if there is an error.
        No return value from this method.

        """
        os.makedirs(self.output, exist_ok=True)

        for i in self._instances:
            i.prepare(copy_all_files=copy_all_files)

        for i in self._instances:
            i.post_prepare()

    def build(self):
        """Build the system.

        Raises an appropriates exception if there is a build error.
        No return value from this method.

        """
        self.generate(copy_all_files=False)
        self._run_action(Builder)

    def load(self):
        """Load the system.

        Raises an appropriate exception if there is a load error.
        No return value from this method.

        """
        self._run_action(Loader)

    def _run_action(self, typ):
        try:
            return self._get_instance_by_type(typ).run()
        except IndexError:
            raise SystemConsistencyError('The system {} is either missing or contains multiple {} modules, so that \
module\'s functionality cannot be invoked.'.format(self, typ.__name__))

    def _get_instance_by_type(self, typ):
        instances = self._get_instances_by_type(typ)
        if len(instances) != 1:
            raise IndexError('This system contains {} instances of {} but exactly one is expected'.
                             format(len(instances), typ.__name__))
        return instances[0]

    def _get_instances_by_type(self, typ):
        return [i for i in self._instances if isinstance(i._module, typ)]

    def __str__(self):
        return "System: %s" % self.name


class Project:
    """The Project is a container for other objects in the system."""

    def __init__(self, filename, search_paths=None):
        """Parses the project definition file `filename` and any imported system and module definition files.

        If filename is None, then a default 'empty' project is created.

        The search path for a project is a list of paths in which modules will be searched.
        The order of the search path matters; the first path in which an entity is found is used.

        The search path consists of the 'user' search paths, and the 'built-in' search paths.
        'user' search paths are searched before 'built-in' search paths.

        The 'user' search paths consist of the 'param' search paths as passed explicitly to the class and
        the 'project' search paths, which are any search paths specified in the project file.
        'param' search paths are searched before the 'project' search paths.
        If no 'param' search paths or 'project' search paths are specified, then the 'user' search path
        defaults to the project file's directory (or the current working directory if no project file is specified.)

        """
        if filename is None:
            self.dom = xml_parse_string('<project></project>')
            self.project_dir = os.getcwd()
        else:
            self.dom = xml_parse_file(filename)
            self.project_dir = os.path.dirname(filename)

        self.entities = {}

        # Find all startup-script items.
        ss_els = self.dom.getElementsByTagName('startup-script')
        for ss in ss_els:
            command = single_text_child(ss)
            ret_code = os.system(command)
            if ret_code != 0:
                err = xml_error_str(ss, "Error running startup-script"
                                        ": '{}' {}".format(command, show_exit(ret_code)))
                raise ProjectStartupError(err)

        param_search_paths = search_paths if search_paths is not None else []

        # Find all search path items, and add to search path
        # All search paths are added before attempting any imports
        project_search_paths = []
        for sp_el in self.dom.getElementsByTagName('search-path'):
            sp = single_text_child(sp_el)
            if sp.endswith('/'):
                sp = sp[:-1]
            project_search_paths.append(sp)

        user_search_paths = param_search_paths + project_search_paths
        if len(user_search_paths) == 0:
            user_search_paths = [self.project_dir]

        built_in_search_paths = []
        if frozen:
            base_file = sys.executable if frozen else __file__
            base_file = follow_link(base_file)

            base_dir = canonical_path(os.path.dirname(base_file))

            def find_share(cur):
                cur = canonical_path(cur)
                maybe_share_path = os.path.join(cur, 'share')
                if os.path.exists(maybe_share_path):
                    return maybe_share_path
                else:
                    up = canonical_path(os.path.join(cur, os.path.pardir))
                    if up == cur:
                        return None
                    return find_share(up)
            share_dir = find_share(base_dir)
            if share_dir is None or not os.path.isdir(share_dir):
                logger.warning("Unable to find 'share' directory.")
            else:
                packages_dir = os.path.join(share_dir, 'packages')
                if not os.path.exists(packages_dir) or not os.path.isdir(packages_dir):
                    logger.warning("Can't find 'packages' directory in '{}'".format(share_dir))
                else:
                    built_in_search_paths.append(packages_dir)

        self.search_paths = user_search_paths + built_in_search_paths

        logger.debug("search_paths %s", self.search_paths)

        output_el = maybe_single_named_child(self.dom, 'output')
        if output_el:
            path = get_attribute(output_el, 'path')
        else:
            path = 'out'
        if os.path.isabs(path):
            self.output = path
        else:
            self.output = os.path.join(self.project_dir, path)

    def entity_name_to_path(self, entity_name):
        """Looks up an entity definition in the search paths by its specified `entity_name`.

        Returns the path to the entity.

        """
        # Search for a given entity name.
        extensions = ['', '.prx', '.py', '.c', '.s', '.asm']

        # Find the first path that exists, we try and load that.  If a
        # path exists, but fails to load for some other reason that is
        # an error, and we don't attempt to load anything else.
        def search_inner(base):
            for ext in extensions:
                path = '%s%s' % (base, ext)
                if os.path.exists(path):
                    return path, ext
            return None, None

        for sp in self.search_paths:
            base = os.path.join(sp, *entity_name.split('.'))
            path, ext = search_inner(base)
            if path is not None:
                break

        if path is None:
            raise EntityNotFound("Unable to find entity named '{}'".format(entity_name))

        if ext == '':
            if not os.path.isdir(path):
                raise EntityNotFound("Unable to find entity named %s" % entity_name)
            # Search for an 'entity.<ext>' file.
            file_path, ext = search_inner(os.path.join(path, 'entity'))
            if file_path is None:
                raise EntityNotFound("Couldn't find entity definition file in '{}'".format(path))
            path = file_path

        return path

    def _parse_import(self, entity_name, path):
        """Parse an entity decribed in the specified path.

        Return the approriate object as determined by the file extension.

        """
        ext = os.path.splitext(path)[1]
        if ext == '.prx':
            return System(entity_name, xml_parse_file(path), self)
        elif ext == '.py':
            py_module = imp.load_source("__prj.%s" % entity_name, path)
            py_module.__path__ = os.path.dirname(py_module.__file__)
            if hasattr(py_module, 'system_build'):
                return Builder(entity_name, py_module)
            elif hasattr(py_module, 'system_load'):
                return Loader(entity_name, py_module)
            elif hasattr(py_module, 'module'):
                return py_module.module
            else:
                raise EntityLoadError("Python entity '%s' from path %s doesn't match any interface" %
                                      (entity_name, path))
        elif ext in ['.c', '.s', '.asm']:
            return SourceModule(entity_name, path)
        else:
            raise EntityLoadError("Unhandled extension '{}'".format(ext))

    def find(self, entity_name):
        """Find an entity (could be a module, system or some other type).

        A KeyError will be raised in the case where the entity can't be found.

        """
        if not valid_entity_name(entity_name):
            # Note: 'entity_name' should be checked before passing to this function.
            raise Exception("Invalid entity name passed to find: '{}'".format(entity_name))
        if entity_name not in self.entities:
            # Try and find the entity name
            path = self.entity_name_to_path(entity_name)
            self.entities[entity_name] = self._parse_import(entity_name, path)

        return self.entities[entity_name]


def generate(args):
    """Genereate the source code for the specified system.

    `args` is expected to provide the following attributes:
    - `project`: an instance of Project.
    - `system`: the name of the system entity to instantiate and generate source.

    This function returns 0 on success and 1 if an error occurs.

    """
    return call_system_function(args, System.generate, extra_args={'copy_all_files': True}, sys_is_path=True)


def build(args):
    """Build the system specified on the command line from its source modules into a binary.

    `args` is expected to provide the following attributes:
    - `project`: an instance of Project
    - `system`: the name of a system entity to instantiate and build

    This function returns 0 on success and 1 if an error occurs.

    """
    return call_system_function(args, System.build)


def load(args):
    """Load the system specified on the command line onto an execution target.

    `args` is expected to provide the following attributes:
    - `project`: an instance of Project
    - `system`: the name of a system entity to instantiate and load

    This function returns 0 on success and 1 if an error occurs.

    """
    return call_system_function(args, System.load)


def call_system_function(args, function, extra_args=None, sys_is_path=False):
    """Instantiate a system and call the given member function of the System class on it."""
    project = args.project
    system_name = args.system

    if extra_args is None:
        extra_args = {}

    try:
        if sys_is_path:
            system_path = system_name
            system_name = os.path.splitext(os.path.basename(system_name))[0]
            print("Loading system: {}".format(system_name))
            system = project._parse_import(system_name, system_path)
            system.output = os.path.curdir
        else:
            if not valid_entity_name(system_name):
                logger.error("System name '{}' is invalid.".format(system_name))
                return 1
            print("Loading system: {}".format(system_name))
            system = project.find(system_name)
    except EntityLoadError:
        logger.error("Unable to load system [{}].".format(system_name))
        return 1
    except EntityNotFound:
        logger.error("Unable to find system [{}].".format(system_name))
        return 1

    if args.output:
        system.output = args.output

    logger.info("Invoking '{}' on system '{}'".format(function.__name__, system.name))
    expected_exceptions = (SystemParseError, SystemLoadError, SystemBuildError, SystemConsistencyError,
                           ResourceNotFoundError, EntityNotFound)
    try:
        function(system, **extra_args)
    except expected_exceptions as e:
        logger.error(str(e))
        return 1

    return 0


SUBCOMMAND_TABLE = {
    'gen': generate,
    'build': build,
    'load': load,
}


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='prj')
    parser.add_argument('--project', default=None,
                        help='project file (project.prj)')
    parser.add_argument('--no-project', action='store_true',
                        help='force no project file')
    parser.add_argument('--search-path', action='append', help='additional search paths')
    parser.add_argument('--verbose', action='store_true', help='provide verbose output')
    parser.add_argument('--output', '-o', help='Output directory')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    build_parser = subparsers.add_parser('gen', help='Generate source code for a system')
    build_parser.add_argument('system', help='system to generate source for')

    build_parser = subparsers.add_parser('build', help='Build a system and create a system image')
    build_parser.add_argument('system', help='system to build')

    load_parser = subparsers.add_parser('load', help='Load a system image onto a device and execute it')
    load_parser.add_argument('system', help='system to load')

    args = parser.parse_args()

    if args.verbose:
        logger.setLevel(_logging.DEBUG)

    if args.command is None:
        parser.print_help()
        parser.exit(1, "\nSee 'prj <subcommand> -h' for more information on a specific command\n")

    if args.command in ['build', 'load'] and args.project is None:
        args.project = 'project.prj'

    if args.no_project:
        args.project = None

    # Initialise project
    try:
        args.project = Project(args.project, args.search_path)
    except (EntityLoadError, EntityNotFound, ProjectStartupError) as e:
        logger.error(str(e))
        return 1
    except FileNotFoundError as e:
        logger.error("Unable to initialise project from file [%s]. Exception: %s" % (args.project, e))
        return 1
    except ExpatError as e:
        logger.error("Parsing %s:%s ExpatError %s" % (e.path, e.lineno, e))
        return 1

    try:
        return SUBCOMMAND_TABLE[args.command](args)
    except EntityLoadError as e:
        logger.error(str(e))
        return 1


def _start():
    developer_mode = 'PRJ_DEVELOP' in os.environ
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        # Ignore Ctrl-C
        pass
    except Exception as e:
        # Any uncaught errors are something we should send to an error dump,
        # if we are in developer mode though, we should enter the debugger.
        if developer_mode:
            pdb.post_mortem()
        else:
            logger.error("An unhandled exception occurred: %s" % e)
            try:
                with open("prj.errors", "w") as f:
                    traceback.print_exception(*sys.exc_info(), file=f)
                logger.error("Please include the 'prj.errors' file when submitting a bug report")
            except:
                pass
            sys.exit(1)

if __name__ == "__main__":
    _start()
