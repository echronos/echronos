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
import shutil
from collections import namedtuple
import pystache
import xml.etree.ElementTree
from .utils import BASE_DIR, base_path, base_to_top_paths
from .cmdline import subcmd


# FIXME: Use correct declaration vs definition.
_REQUIRED_H_SECTIONS = ['public_headers',
                        'public_types',
                        'public_structures',
                        'public_object_like_macros',
                        'public_function_like_macros',
                        'public_state',
                        'public_function_declarations']

_REQUIRED_C_SECTIONS = ['headers',
                        'object_like_macros',
                        'types',
                        'structures',
                        'extern_declarations',
                        'function_declarations',
                        'state',
                        'function_like_macros',
                        'functions',
                        'public_functions']

_REQUIRED_DEP_SECTIONS = ['provides', 'requires']

_REQUIRED_DOC_SECTIONS = ['doc_header', 'doc_concepts', 'doc_api',
                          'doc_configuration', 'doc_footer']


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


def _sort_typedefs(typedef_lines):
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


def _parse_sectioned_file(fn, config, required_sections):
    """Given a sectioned C-like file, returns a dictionary of { section: content }

    For example an input of:
    /*| foo |*/
    foo data....

    /*| bar |*/
    bar data....

    Would produce:

    { 'foo' : "foo data....", 'bar' : "bar data...." }
    """
    if not os.path.exists(fn):
        # Skip non-existent files
        return None

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

    for s in required_sections:
        if s not in sections:
            raise Exception("Couldn't find expected section '{}' in file: '{}'".format(s, fn))

    return sections


def _get_sections(bound_components, filename, sections):
    return [_parse_sectioned_file(os.path.join(bc.path, filename), bc.config, sections) for bc in bound_components
            if os.path.exists(os.path.join(bc.path, filename))]


_BoundComponent = namedtuple("_BoundComponent", ['path', 'config'])


class Component(namedtuple('Component', ['name', 'configuration', 'pkg_component'])):
    def __new__(cls, name, configuration={}, pkg_component=False):
        return super(Component, cls).__new__(cls, name, configuration, pkg_component)


def _bind_components(components, pkg_name, search_paths):
    # Locate all component directories
    # [Component] -> [_BoundComponent]
    bound_components = []
    for component in components:
        if component.pkg_component:
            resource_name = '{0}-{1}'.format(component.name, pkg_name)
        else:
            resource_name = component.name

        for search_path in search_paths:
            path = os.path.join(search_path, resource_name)
            if os.path.exists(path):
                break
        else:
            raise KeyError('Unable to find component "{}" in {}'.format(resource_name, search_paths))
        bound_components.append(_BoundComponent(path, component.configuration))
    return bound_components


def _generate(rtos_name, components, pkg_name, search_paths):
    """Generate the RTOS module to disk, so it is available as a compile and link unit to projects.

    packages/<pkg_name>/rtos-<rtos_name>/rtos-<rtos_name>.c
                                        /rtos-<rtos_name>.h
                                        /schema.xml
                                        /entity.py
    """

    bound_components = _bind_components(components, pkg_name, search_paths)

    # Create module name and output directory
    module_name = 'rtos-' + rtos_name
    module_dir = base_path('packages', pkg_name, module_name)
    os.makedirs(module_dir, exist_ok=True)

    # Generate .c file
    all_c_sections = _get_sections(bound_components, "implementation.c", _REQUIRED_C_SECTIONS)
    source_output = os.path.join(module_dir, module_name + '.c')
    with open(source_output, 'w') as f:
        for ss in _REQUIRED_C_SECTIONS:
            data = "\n".join(c_sections[ss] for c_sections in all_c_sections)
            if ss == 'types':
                data = _sort_typedefs(data)
            f.write(data)
            f.write('\n')

    # Generate .h file
    all_h_sections = _get_sections(bound_components, "header.h", _REQUIRED_H_SECTIONS)
    header_output = os.path.join(module_dir, module_name + '.h')
    with open(header_output, 'w') as f:
        mod_name = module_name.upper().replace('-', '_')
        f.write("#ifndef {}_H\n".format(mod_name))
        f.write("#define {}_H\n".format(mod_name))
        for ss in _REQUIRED_H_SECTIONS:
            if ss == 'public_function_declarations':
                f.write("#ifdef __cplusplus\nextern \"C\" {\n#endif\n")
            f.write("\n".join(h_sections[ss] for h_sections in all_h_sections) + "\n")
            if ss == 'public_function_declarations':
                f.write("#ifdef __cplusplus\n}\n#endif\n")
        f.write("\n#endif /* {}_H */".format(mod_name))

    # Generate docs
    if os.path.exists(os.path.join(BASE_DIR, 'components', rtos_name, 'docs.md')):
        all_doc_sections = _get_sections(bound_components, "docs.md", _REQUIRED_DOC_SECTIONS + _REQUIRED_DEP_SECTIONS)
        all_doc_sections = _sort_sections_by_dependencies(bound_components, all_doc_sections)

        doc_output = os.path.join(module_dir, 'docs.md')
        with open(doc_output, 'w') as f:
            for ss in _REQUIRED_DOC_SECTIONS:
                data = "\n\n".join(doc_sections[ss] for doc_sections in all_doc_sections if doc_sections is not None)
                f.write('\n')
                f.write(data)
                f.write('\n')

        output_dir = os.path.join(module_dir, 'docs')
        if os.path.exists(output_dir):
            shutil.rmtree(output_dir, ignore_errors=True)
        for bc in bound_components:
            input_dir = os.path.join(bc.path, 'docs')
            if os.path.isdir(input_dir):
                # recursively copy contents of input_dir into output_dir
                # shutil.copytree() cannot be used because it requires the destination to not yet exist
                for parent, dirs, files in os.walk(input_dir):
                    for file in files:
                        src = os.path.join(parent, file)
                        dst = os.path.join(output_dir, os.path.relpath(src, input_dir))
                        if os.path.exists(dst):
                            print('Warning: the file {} overwrites the file {} which originates from a different \
component'.format(src, dst))
                        os.makedirs(os.path.dirname(dst), exist_ok=True)
                        shutil.copy(src, dst)

    # Generate .xml file
    config_output = os.path.join(module_dir, 'schema.xml')
    with open(config_output, 'w') as f:
        f.write('<?xml version="1.0" encoding="UTF-8" ?>\n')
        xml_files = [os.path.join(bc.path, "schema.xml") for bc in bound_components]
        schema = _merge_schema_files(xml_files)
        f.write(schema)

    # Generate .py file
    python_output = os.path.join(module_dir, 'entity.py')
    python_file = os.path.join(BASE_DIR, 'components', '{}.py'.format(rtos_name))
    shutil.copyfile(python_file, python_output)


def _get_search_paths(topdir):
    """Find and return the directories that, by convention, are expected to contain component modules.

    As search directories qualify all directories called 'components' in the BASE_DIR or its parent directories.
    The search for such directories upwards in the directory tree from BASE_DIR stops at the first parent
    directory not containing a 'components' directory.

    """
    paths = list(base_to_top_paths(topdir, 'components'))
    paths.reverse()
    return paths


@subcmd(name='packages',
        cmd='build',
        help='Generate packages from components')
def build(args):
    # Generate RTOSes
    search_paths = _get_search_paths(args.topdir)
    for pkg_name, rtos_names in args.configurations.items():
        for rtos_name in rtos_names:
            _generate(rtos_name, args.skeletons[rtos_name], pkg_name, search_paths)
    return 0


_DependencyNode = namedtuple("_DependencyNode", ('provides', 'requires'))


class _UnresolvableDependencyError(Exception):
    pass


def _sort_by_dependencies(nodes, ignore_cyclic_dependencies=False):
    todo = list(nodes)
    provided = set()

    while todo:
        for idx, node in enumerate(todo):
            if set(node.requires).issubset(provided):
                del todo[idx]
                for p in node.provides:
                    provided.add(p)
                yield node
                break
        else:
            if ignore_cyclic_dependencies:
                all_provided = set(provided)
                todo_required = set()
                for node in todo:
                    all_provided.update(set(node.provides))
                    todo_required.update(set(node.requires))
                if todo_required.issubset(all_provided):
                    yield from todo
                    return
                else:
                    msg = 'The nodes {} have the unresolvable dependencies {}.'.format(todo,
                                                                                       todo_required - all_provided)
            else:
                msg = 'The dependencies of the nodes {} cannot be resolved.'.format(todo)
            raise _UnresolvableDependencyError(msg)


def _sort_sections_by_dependencies(components, sections):
    class _Node:
        def __init__(self, path, section):
            self.path = path
            self.section = section
            self.provides = section['provides'].split()
            self.requires = section['requires'].split()

        def __repr__(self):
            return '{}, provides: {}, requires: {}'.format(self.path, self.provides, self.requires)

    nodes = [_Node(os.path.basename(component.path), section) for component, section in
             zip(components, sections) if section]
    return [node.section for node in _sort_by_dependencies(nodes, ignore_cyclic_dependencies=True)]
