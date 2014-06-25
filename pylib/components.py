import os
import shutil
from collections import namedtuple
import pystache
import xml.etree.ElementTree
from .utils import BASE_DIR, base_path


# FIXME: Use correct declaration vs definition.
_REQUIRED_H_SECTIONS = ['public_headers',
                        'public_type_definitions',
                        'public_structure_definitions',  # FIXME: not collected!
                        'public_object_like_macros',
                        'public_function_like_macros',
                        'public_extern_definitions',
                        'public_function_definitions',
                        ]

_REQUIRED_C_SECTIONS = ['headers',
                        'object_like_macros',
                        'type_definitions',
                        'structure_definitions',
                        'extern_definitions',
                        'function_definitions',
                        'state',
                        'function_like_macros',
                        'functions',
                        'public_functions']


class _SchemaFormatError(RuntimeError):
    """To be raised when a component configuration schema violates assumptions or conventions."""
    pass


def _merge_schema_entries(a, b, path=''):
    """Recursively merge the entries of two XML component schemas.

    'a' and 'b' (instances of xml.etree.ElementTree.Element) are the two schema entries to merge.
    All entries from 'b' are merged into 'a'.
    If 'a' contains an entry a* with the same name as an entry b* in 'b', they can only be merged if both a* and b*
    have child entries themselves.
    If either a* or b* does not have at least one child entry, this function raises a _SchemaFormatError.

    Within each of 'a' and 'b', the names of their entries must be unique.
    In other words, no two entries in 'a' may have the same name.
    The same applies to 'b'.

    When the function returns, 'a' contains all entries from 'b' and 'b' is unmodified.

    """
    a_children = {child.attrib['name']: child for child in a}
    for b_child in b:
        try:
            name = b_child.attrib['name']
        except KeyError:
            raise _SchemaFormatError('A schema entry under "{}" does not contain a name attribute'.format(path))
        if name in a_children:
            try:
                a_child = a_children[name]
            except KeyError:
                raise _SchemaFormatError('A schema entry under "{}" does not contain a name attribute'.format(path))
            if (len(b_child) == 0) != (len(a_child) == 0):
                raise _SchemaFormatError('Unable to merge two schemas: \
the entry {}.{} is present in both schemas, but it has children in one and no children in the other. \
To merge two schemas, corresponding entries both need need to either have child entries or not.'.format(path, name))
            if len(b_child) and len(a_child):
                _merge_schema_entries(a_child, b_child, '{}.{}'.format(path, name))
            else:
                # replace existing entry in a with the entry from b, allowing to override entries
                a.remove(a_child)
                a.append(b_child)
        else:
            a.append(b_child)


def _merge_schema_files(xml_files):
    merged_schema = xml.etree.ElementTree.fromstring('<schema>\n</schema>')

    sections = [open(xml_file).read().strip() for xml_file in xml_files if os.path.exists(xml_file)]
    for section in sections:
        schema = xml.etree.ElementTree.fromstring('<schema>\n{}\n</schema>'.format(section))
        _merge_schema_entries(merged_schema, schema)

    return xml.etree.ElementTree.tostring(merged_schema).decode()


def sort_typedefs(typedef_lines):
    """Given a string containing multiple lines of typedefs, sort the lines so that the typedefs are in the 'correct'
    order.

    The typedef lines must only contain typedefs.
    No comments or other data is allowed.
    Blank lines are allowed, but will be ommited from the output.

    The correct order for typedefs is on such that if typedef 'b' is defined in terms of typedef 'a', typedef 'a' will
    appear first in the sorted output.

    """

    typedefs = []
    for l in typedef_lines.split('\n'):
        if l == '':
            continue
        if not l.endswith(';'):
            raise Exception("Expect a typedef line to end with ';' ({})".format(l))
        parts = l[:-1].split()
        if not parts[0] == 'typedef':
            raise Exception("Expect typedef line to startwith 'typedef'")
        new_type = parts[-1]
        old_type = ' '.join(parts[1:-1])
        typedefs.append((new_type, old_type))

    new_types = [new for (new, _) in typedefs]
    r = []

    # First put in any types that don't cross reference.
    #  we assume they are defined in other headers.
    for (new, old) in typedefs[:]:
        if old not in new_types:
            r.append((new, old))
            typedefs.remove((new, old))

    # Now, for each new type
    i = 0
    while i < len(r):
        check_type = r[i][0]
        i += 1
        for (new, old) in typedefs[:]:
            if old == check_type:
                r.append((new, old))
                typedefs.remove((new, old))

    return '\n'.join(['typedef {} {};'.format(old, new) for (new, old) in r])


def _render_data(in_data, name, config):
    """Render input data (`in_data`) using a given `config`. The result is returned."""
    pystache.defaults.MISSING_TAGS = 'strict'
    pystache.defaults.DELIMITERS = ('[[', ']]')
    pystache.defaults.TAG_ESCAPE = lambda u: u
    return pystache.render(in_data, config, name=name)


def _parse_sectioned_file(fn, config, required_components):
    """Given a sectioned C-like file, returns a dictionary of { section: content }

    For example an input of:
    /*| foo |*/
    foo data....

    /*| bar |*/
    bar data....

    Would produce:

    { 'foo' : "foo data....", 'bar' : "bar data...." }
    """

    with open(fn) as f:
        sections = {}
        current_lines = None
        for line in f.readlines():
            line = line.rstrip()

            if line.startswith('/*|') and line.endswith('|*/'):
                section = line[3:-3].strip()
                current_lines = []
                sections[section] = current_lines
            elif current_lines is not None:
                current_lines.append(line)

    for key, value in sections.items():
        sections[key] = _render_data('\n'.join(value).rstrip(), "{}: Section {}".format(fn, key), config)

    for s in required_components:
        if s not in sections:
            raise Exception("Couldn't find expected section '{}' in file: '{}'".format(s, fn))

    return sections


class Component:
    """Represents an optional, exchangeable piece of functionality of an RTOS.

    Components reside in the components/ directory of the core or sub-projects.
    This class transparently finds components in any of the available core or sub-projects.
    Instances of this class encapsulate the act of parsing a component file and converting it into configuration data
    used when generating an RtosModule.

    """
    _search_paths = None

    @staticmethod
    def _get_search_paths():
        search_paths = []

        current_dir = BASE_DIR
        while True:
            components_dir = os.path.join(current_dir, 'components')
            if os.path.isdir(components_dir):
                search_paths.append(components_dir)
                next_dir = os.path.dirname(current_dir)
                if next_dir != current_dir:
                    current_dir = next_dir
                else:
                    break
            else:
                break

        # reverse the search paths so that they are sorted by increasing depth in the directory tree
        # this is expected to lead to components in client repositories to override those in the core repository
        search_paths.reverse()

        return search_paths

    @staticmethod
    def get_search_paths():
        """Find and return the directories that, by convention, are expected to contain component modules.

        As search directories qualify all directories called 'components' in the BASE_DIR or its parent directories.
        The search for such directories upwards in the directory tree from BASE_DIR stops at the first parent
        directory not containing a 'components' directory.

        """
        if Component._search_paths is None:
            Component._search_paths = Component._get_search_paths()
        return Component._search_paths

    @staticmethod
    def _find(partial_path):
        """Find the component partial_path in the core repository or client repositories further up in the directory
        tree."""
        for search_path in Component.get_search_paths():
            component_path = os.path.join(search_path, partial_path)
            if os.path.exists(component_path):
                return component_path
        raise KeyError('Unable to find component "{}"'.format(partial_path))

    def __init__(self, name, configuration={}, arch_component=False):
        """Create a component object.

        Such objects encapsulate the act of parsing a corresponding source file.
        The parsed data is converted into configuration information used when generating an RtosModule by rendering an
        RTOS template file.

        'name' is the base name of the source file of this component that is parsed to obtain this
        component's properties.
        For example, the base name of the interrupt event component is 'interrupt-event', which expands to the on-disk
        file name of components/interrupt-event/interrupt-event.c.

        'configuration' is a dictionary with configuration information.
        It is passed to the '_parse_sectioned_file()' function used to parse this component's source file.

        'arch_component' is a boolean indicating whether this component is architecture dependent.
        """
        self._base_name = name
        self._resource_name = name
        self._configuration = configuration
        self._arch_component = arch_component

    def bind(self, arch_name):
        if self._arch_component:
            self._resource_name = '{0}-{1}'.format(self._base_name, arch_name)
        self._path = Component._find(self._resource_name)

        self.c_sections = self._parse(".c", _REQUIRED_C_SECTIONS, self._path)
        self.h_sections = self._parse(".h", _REQUIRED_H_SECTIONS, self._path)
        self.xml_file = os.path.join(self._path, 'schema.xml')

    def _parse(self, ext, required_components, path):
        """Retrieve the properties of this component by parsing its corresponding source file.

        This function returns a dictionary containing configuration information that can be used to render an RTOS
        template.

        """
        component = os.path.join(path, '{0}{1}'.format(self._resource_name, ext))
        return _parse_sectioned_file(component, self._configuration, required_components)


def _generate(rtos_name, components, arch_name):
    """Generate the RTOS module to disk, so it is available as a compile and link unit to projects."""

    # Set up the data structures for all the components
    for component in components:
        component.bind(arch_name)

    module_name = 'rtos-' + rtos_name
    module_dir = base_path('packages', arch_name, module_name)

    os.makedirs(module_dir, exist_ok=True)
    python_output = os.path.join(module_dir, 'entity.py')
    source_output = os.path.join(module_dir, module_name + '.c')
    header_output = os.path.join(module_dir, module_name + '.h')
    config_output = os.path.join(module_dir, 'schema.xml')

    source_sections = ['headers', 'object_like_macros',
                       'type_definitions', 'structure_definitions',
                       'extern_definitions', 'function_definitions',
                       'state', 'function_like_macros',
                       'functions', 'public_functions']
    header_sections = ['public_headers', 'public_type_definitions',
                       'public_object_like_macros', 'public_function_like_macros',
                       'public_extern_definitions', 'public_function_definitions']

    with open(source_output, 'w') as f:
        for ss in source_sections:
            data = "\n".join(comp.c_sections[ss] for comp in components)
            if ss == 'type_definitions':
                data = sort_typedefs(data)
            f.write(data)
            f.write('\n')

    with open(header_output, 'w') as f:
        mod_name = module_name.upper().replace('-', '_')
        f.write("#ifndef {}_H\n".format(mod_name))
        f.write("#define {}_H\n".format(mod_name))
        for ss in header_sections:
            f.write("\n".join(comp.h_sections[ss] for comp in components) + "\n")
        f.write("\n#endif /* {}_H */".format(mod_name))

    with open(config_output, 'w') as f:
        f.write('''<?xml version="1.0" encoding="UTF-8" ?>
''')
        schema = _merge_schema_files([comp.xml_file for comp in components])
        f.write(schema)

    python_file = os.path.join(BASE_DIR, 'components', '{}.py'.format(rtos_name))
    shutil.copyfile(python_file, python_output)


def build(args):
    # Generate RTOSes
    for rtos_name, arch_names in args.configurations.items():
        generate_rtos_module(rtos_name, args.skeletons[rtos_name], arch_names)


def generate_rtos_module(rtos_name, components, arch_names):
    """Generate RTOS modules for several architectures from a given skeleton."""
    for arch_name in arch_names:
        _generate(rtos_name, components, arch_name)
