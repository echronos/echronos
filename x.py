#!/usr/bin/env python3.3
"""
Overview
---------
`x.py` is the main *project management script* for the RTOS project.
As a *project magement script* it should handle any actions related to working on the project, such as building
artifacts for release, project management related tasks such as creating reviews, and similar task.
Any project management related task should be added as a subcommand to `x.py`, rather than adding another script.

Released Files
---------------

One of the main tasks of `x.py` is to create the releasable artifacts (i.e.: things that will be shipped to users).

### `prj` release information

prj will be distributed in source format for now as the customer likes it that way, and also because of the
impracticality of embedding python3 into a distributable .exe .
The enduser will need to install Python 3.3.
The tool can be embedded (not installed) into a project tree (i.e.: used inplace).

### Package release information

Numerous *packages* will be release to the end user.

One or more RTOS packages will be released.
These include:
* The RTOS core
* The RTOS optional-modules
* Test Suite
* Documentation (PDF)

Additionally one or more build modules will be released.
These include:
* Module
* Documentation

### Built files

The following output files will be produced by `x.py`.

* release/prj-<version>.zip
* release/<rtos-foo>-<version>.zip
* release/<build-name>-<version>.zip

Additional Requirements
-----------------------

This `x.py` tool shouldn't leave old files around (like every other build tool on the planet.)
So, if `x.py` is building version 3 of a given release, it should ensure old releases are not still in the `releases`
directory.

"""
import sys
import os

# FIXME: Use correct declaration vs definition.
REQUIRED_COMPONENT_SECTIONS = ['public_headers',
                               'public_type_definitions',
                               'public_structure_definitions',
                               'public_object_like_macros',
                               'public_function_like_macros',
                               'public_extern_definitions',
                               'public_function_definitions',
                               'headers',
                               'object_like_macros',
                               'type_definitions',
                               'structure_definitions',
                               'extern_definitions',
                               'function_definitions',
                               'state',
                               'function_like_macros',
                               'functions',
                               'public_functions']

externals = ['pep8', 'nose', 'ice']
from pylib.utils import BASE_DIR

sys.path = [os.path.join(BASE_DIR, 'external_tools', e) for e in externals] + sys.path
sys.path.insert(0, os.path.join(BASE_DIR, 'prj/app/pystache'))
if __name__ == '__main__':
    sys.modules['x'] = sys.modules['__main__']

### Check that the correct Python is being used.
correct = None
if sys.platform == 'darwin':
    correct = os.path.abspath(os.path.join(BASE_DIR, 'tools/x86_64-apple-darwin/bin/python3.3'))
elif sys.platform.startswith('linux'):
    correct = os.path.abspath(os.path.join(BASE_DIR, 'tools/x86_64-unknown-linux-gnu/bin/python3.3'))

if correct is not None and sys.executable != correct:
    print("x.py expects to be executed using {} (not {}).".format(correct, sys.executable), file=sys.stderr)
    sys.exit(1)

import argparse
import calendar
import glob
import inspect
import io
import ice
import logging
import pystache
import signal
import shutil
import subprocess
import tarfile
import tempfile
import xml.etree.ElementTree
import zipfile
from contextlib import contextmanager
from glob import glob

from pylib.tasks import new_review, new_task, tasks, integrate, Git
from pylib.tests import prj_test, x_test, pystache_test, rtos_test, check_pep8

# Set up a specific logger with our desired output level
logger = logging.getLogger()
logger.setLevel(logging.INFO)


BASE_TIME = calendar.timegm((2013, 1, 1, 0, 0, 0, 0, 0, 0))


# topdir is the rtos repository directory in which the user invoked the x tool.
# If the x tool is invoked from a client repository through a wrapper, topdir contains the directory of that client
# repository.
# If the user directly invokes x tool of the RTOS core, topdir is the directory of this file.
# topdir defaults to the core directory.
# It may be modified by an appropriate invocation of main().
topdir = os.path.normpath(os.path.dirname(__file__))


SIG_NAMES = dict((k, v) for v, k in signal.__dict__.items() if v.startswith('SIG'))


def show_exit(exit_code):
    sig_num = exit_code & 0xff
    exit_status = exit_code >> 8
    if sig_num == 0:
        return "exit: {}".format(exit_status)
    else:
        return "signal: {}".format(SIG_NAMES.get(sig_num, 'Unknown signal {}'.format(sig_num)))


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


@contextmanager
def chdir(path):
    """Current-working directory context manager.

    Makes the current working directory the specified `path` for the duration of the context.

    Example:

    with chdir("newdir"):
        # Do stuff in the new directory
        pass

    """
    cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(cwd)


@contextmanager
def tempdir():
    tmpdir = tempfile.mkdtemp()
    try:
        yield tmpdir
    finally:
        shutil.rmtree(tmpdir)


def walk(path, flt=None):
    """Return a list of all files in a given path. flt can be used to filter
    any unwanted files."""
    def always_true(x):
        return True

    if flt is None:
        flt = always_true

    file_list = []
    for root, dirs, files in os.walk(path):
        file_list.extend([os.path.join(root, f) for f in files if not flt(os.path.join(root, f))])
    return file_list


def base_path(*path):
    """Join one or more pathname components to the directory in which the
    script resides.

    The goal of this script is to easily allow pathnames that are relative
    to the directory in which the script resides.

    If the script is run as `./x.py` `base_path('foo')` will return
    ./foo.

    If the script is run by an absolute path (e.g.: `/path/to/x.py`)
    `base_path('foo')` will return `/path/to/foo`.

    If user is in the `./bar` directory and runs the script as
    `../x.py`, `base_path('foo')` will return `../bar`.

    The path returned by `base_path` will allow access to the file
    assuming that the current working directory has not been changed.
    """
    return os.path.join(BASE_DIR, *path)


def top_path(*path):
    """Return a path relative to the directory in which the x tool or wrapper was invoked.

    This function is equivalent to base_path(), except when the x tool is invoked in a client repository through a
    wrapper.
    In that case, the specified path is not appended to the directory containing the core x.py file, but the directory
    containing the wrapper x.py file invoked by the user.

    """
    return os.path.join(topdir, *path)


def base_to_top_paths(*path):
    """For each directory from BASE_DIR up to topdir in the directory tree, append the specified path and return the
    resulting sequence.

    For example, if topdir is '/rtos/', BASE_DIR is '/rtos/core/', and *path is ['packages'], this function returns
    ['/rtos/core/packages', '/rtos/packages']

    If topdir equals BASE_DIR, the result of this function is a sequence with a single element and equal to
    [base_path(*path)]

    """
    result = []

    cur_dir = os.path.abspath(BASE_DIR)
    stop_dir = os.path.abspath(topdir)
    iterate = True
    while iterate:
        result.append(os.path.join(cur_dir, *path))
        iterate = (cur_dir != stop_dir)
        cur_dir = os.path.dirname(cur_dir)

    return result


def un_base_path(path):
    """Reverse the operation performed by `base_path`.

    For all `x`, `un_base_path(base_path(x)) == x`.
    """
    if BASE_DIR == '':
        return path
    else:
        return path[len(BASE_DIR) + 1:]


def get_host_platform_name():
    if sys.platform == 'darwin':
        return 'x86_64-apple-darwin'
    elif sys.platform == 'linux':
        return 'x86_64-unknown-linux-gnu'
    elif sys.platform == 'win32':
        return 'win32'
    else:
        raise RuntimeError('Unsupported platform {}'.format(sys.platform))


_executable_extension = None


def get_executable_extension():
    global _executable_extension
    if _executable_extension is None:
        _executable_extension = {'darwin': '',
                                 'linux': '',
                                 'win32': '.exe',
                                 }[sys.platform]
    return _executable_extension


class SchemaFormatError(RuntimeError):
    """To be raised when a component configuration schema violates assumptions or conventions."""
    pass


def merge_schema_entries(a, b, path=''):
    """Recursively merge the entries of two XML component schemas.

    'a' and 'b' (instances of xml.etree.ElementTree.Element) are the two schema entries to merge.
    All entries from 'b' are merged into 'a'.
    If 'a' contains an entry a* with the same name as an entry b* in 'b', they can only be merged if both a* and b*
    have child entries themselves.
    If either a* or b* does not have at least one child entry, this function raises a SchemaFormatError.

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
            raise SchemaFormatError('A schema entry under "{}" does not contain a name attribute'.format(path))
        if name in a_children:
            try:
                a_child = a_children[name]
            except KeyError:
                raise SchemaFormatError('A schema entry under "{}" does not contain a name attribute'.format(path))
            if (len(b_child) == 0) != (len(a_child) == 0):
                raise SchemaFormatError('Unable to merge two schemas: \
the entry {}.{} is present in both schemas, but it has children in one and no children in the other. \
To merge two schemas, corresponding entries both need need to either have child entries or not.'.format(path, name))
            if len(b_child) and len(a_child):
                merge_schema_entries(a_child, b_child, '{}.{}'.format(path, name))
            else:
                # replace existing entry in a with the entry from b, allowing to override entries
                a.remove(a_child)
                a.append(b_child)
        else:
            a.append(b_child)


def merge_schema_sections(sections):
    merged_schema = xml.etree.ElementTree.fromstring('<schema>\n</schema>')

    for section in sections:
        schema = xml.etree.ElementTree.fromstring('<schema>\n{}\n</schema>'.format(section))
        merge_schema_entries(merged_schema, schema)

    return xml.etree.ElementTree.tostring(merged_schema).decode()


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
    def find(partial_path):
        """Find the component partial_path in the core repository or client repositories further up in the directory
        tree."""
        for search_path in Component.get_search_paths():
            component_path = os.path.join(search_path, partial_path)
            if os.path.exists(component_path):
                return component_path
        raise KeyError('Unable to find component "{}"'.format(partial_path))

    def __init__(self, name, resource_name=None, configuration={}):
        """Create a component object.

        Such objects encapsulate the act of parsing a corresponding source file.
        The parsed data is converted into configuration information used when generating an RtosModule by rendering an
        RTOS template file.

        'name' is the component name used in the RTOS template file.
        For example, the properties of the interrupt event component are referred to as 'interrupt_event.xyz' in the
        RTOS template files.

        'resource_name' is the base name of the source file of this component that is parsed to obtain this
        component's properties.
        For example, the base name of the interrupt event component is 'interrupt-event', which expands to the on-disk
        file name of interrupt-event.c.

        'configuration' is a dictionary with configuration information.
        It is passed to the 'parse_sectioned_file()' function used to parse this component's source file.

        """
        self.name = name
        if resource_name is not None:
            self._resource_name = resource_name
        else:
            self._resource_name = name
        self._configuration = configuration

    def parse(self, parsing_configuration={}):
        """Retrieve the properties of this component by parsing its corresponding source file.

        'parsing_configuration' is an optional dictionary that is merged with the component's base configuration and
        passed to the parsing function.

        This function returns a dictionary containing configuration information that can be used to render an RTOS
        template.

        """
        if isinstance(parsing_configuration, dict):
            configuration = self._configuration.copy()
            configuration.update(parsing_configuration)
        else:
            configuration = self._configuration

        component = None
        for name in [f.format(self._resource_name) for f in ['{0}.c', '{0}/{0}.c']]:
            try:
                component = Component.find(name)
                break
            except KeyError:
                pass
        if component is None:
            raise KeyError('Unable to find component "{}"'.format(self._resource_name))

        return parse_sectioned_file(component, configuration)


class ArchitectureComponent(Component):
    """This refinement of the Component class represents an architecture-specific component.

    This class encapsulates the act of finding the architecture-specific source file corresponding to this component.
    This is opposed to the base Component class which is unaware of architecture-specific file naming conventions.

    """
    def parse(self, arch):
        """Retrieve the properties of this component by parsing its architecture-specific source file.

        'arch', an instance of Architecture, identifies the architecture of the source file to parse.

        Otherwise, this function behaves as Component.parse().

        """
        assert isinstance(arch, Architecture)

        component = None
        for name in [f.format(arch.name, self._resource_name) for f in ['{0}-{1}/{0}-{1}.c', '{1}-{0}.c']]:
            try:
                component = Component.find(name)
                break
            except KeyError:
                pass
        if component is None:
            raise KeyError('Unable to find component "{}" for architecture {}'.format(self._resource_name, arch.name))

        return parse_sectioned_file(component, self._configuration)


def prj_build(args):
    host = get_host_platform_name()
    if sys.platform == 'darwin':
        extras = ['-framework', 'CoreFoundation', '-lz']
    elif sys.platform == 'linux':
        extras = ['-lz', '-lm', '-lpthread', '-lrt', '-ldl', '-lcrypt', '-lutil']
    elif sys.platform == 'win32':
        pass
    else:
        print("Building prj currently unsupported on {}".format(sys.platform))
        return 1

    prj_build_path = top_path('prj_build_{}'.format(host))
    os.makedirs(prj_build_path, exist_ok=True)

    if sys.platform == 'win32':
        prj_build_win32(prj_build_path)
    else:
        with chdir(prj_build_path):
            ice.create_lib('prj', '../prj/app', main='prj')
            ice.create_lib('prjlib', '../prj/app/lib')
            ice.create_lib('pystache', '../prj/app/pystache',
                           excluded=['setup', 'pystache.tests', 'pystache.commands'])
            ice.create_lib('ply', '../prj/app/ply', excluded=['setup'])
            ice.create_stdlib()
            ice.create_app(['stdlib', 'prj', 'prjlib', 'pystache', 'ply'])

            cmd = ['gcc', '*.c', '-o', 'prj', '-I../tools/include/python3.3m/',
                   '-I../tools/{}/include/python3.3m/'.format(host),
                   '-L../tools/{}/lib/python3.3/config-3.3m'.format(host),
                   '-lpython3.3m']
            cmd += extras

            cmd = ' '.join(cmd)
            r = os.system(cmd)
            if r != 0:
                print("Error building {}. cmd={}. ".format(show_exit(r), cmd))


def prj_build_win32(output_dir):
    """Create a distributable version of prj.py.

    We currently do not have the infrastructure in place to statically compile and link prj.py and its dependencies
    against the complete python interpreter.

    However, it is still desirable to create only a single resource that can stand alone given an installed python
    interpreter.
    Therefore, collect prj and its dependencies in a zip file that is executable by the python interpreter.

    """
    with zipfile.ZipFile(os.path.join(output_dir, 'prj'), mode='w') as zip:
        top = os.path.abspath(base_path('prj', 'app'))
        top_len = len(top)
        for dir_path, dir_names, file_names in os.walk(top):
            archive_dir_path = os.path.relpath(dir_path, top)
            for file_name in file_names:
                file_path = os.path.join(dir_path, file_name)
                if dir_path == top and file_name == 'prj.py':
                    # The python interpreter expects to be informed about the main file in the zip file by naming it
                    # __main__.py
                    archive_file_path = os.path.join(archive_dir_path, '__main__.py')
                else:
                    archive_file_path = os.path.join(archive_dir_path, file_name)
                zip.write(file_path, archive_file_path)
    with open(os.path.join(output_dir, 'prj.bat'), 'w') as f:
        f.write('@ECHO OFF\npython %~dp0\\prj')


def build(args):
    # Generate RTOSes
    for rtos_name, arch_names in configurations.items():
        generate_rtos_module(skeletons[rtos_name], [architectures[arch] for arch in arch_names])


def parse_sectioned_file(fn, config={}):
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
        sections[key] = render_data('\n'.join(value).rstrip(), "{}: Section {}".format(fn, key), config)

    for s in REQUIRED_COMPONENT_SECTIONS:
        if s not in sections:
            raise Exception("Couldn't find expected section '{}' in file: '{}'".format(s, fn))

    return sections


def render_data(in_data, name, config):
    """Render input data (`in_data`) using a given `config`. The result is returned."""
    pystache.defaults.MISSING_TAGS = 'strict'
    pystache.defaults.DELIMITERS = ('[[', ']]')
    pystache.defaults.TAG_ESCAPE = lambda u: u
    return pystache.render(in_data, config, name=name)


class Architecture:
    """Represents the properties of a target architecture for which an RtosModule can be generated."""
    def __init__(self, name, configuration):
        assert isinstance(name, str)
        assert isinstance(configuration, dict)
        self.name = name
        self.configuration = configuration


class RtosSkeleton:
    """Represents an RTOS variant as defined by a set of components / functionalities.

    For example, the specific RTOS variant gatria consists exactly of a context-switch and a scheduler component.

    This class encapsulates the act of deriving an RtosModule for a specific configuration and target architecture.

    """
    def __init__(self, name, components, configuration={}):
        """Create an RTOS skeleton based on its core properties.

        'name', a string, is the unique name of the RTOS skeleton.

        'components', a sequence of Component instances, is the set of components that define this RTOS variant.

        'configuration', a dictionary, contains configuration information specific to this RTOS variant.
        It is used when generating an RtosModule from this skeleton.

        """
        assert isinstance(name, str)
        assert isinstance(components, list)
        assert isinstance(configuration, dict)
        self.name = name
        self.python_file = os.path.join(BASE_DIR, 'components', '{}.py'.format(self.name))

        self._components = components
        self._configuration = configuration

    def get_module_sections(self, arch):
        """Retrieve the sections necessary to generate an RtosModule from this skeleton.

        """
        module_sections = {}
        for component in self._components:
            for name, contents in component.parse(arch).items():
                if name not in module_sections:
                    module_sections[name] = []
                module_sections[name].append(contents)
        return module_sections

    def create_configured_module(self, arch):
        """Retrieve module configuration information and create a corresponding RtosModule instance.

        This is only a convenience function.

        """
        return RtosModule(self.name, arch, self.get_module_sections(arch), self.python_file)


class RtosModule:
    """Represents an instance of an RtosSkeleton (or RTOS variant) with a specific configuration, in particular for a
    specific target architecture.

    This class encapsulates the act of rendering an RTOS template given an RTOS configuration into a module on disk.

    """
    def __init__(self, name, arch, sections, python_file):
        """Create an RtosModule instance.

        'name', a string, is the name of the RTOS, i.e., the same as the underlying RtosSkeleton.

        'arch', an instance of Architecture, is the architecture this module is targetted for.

        'sections', a dictionary, containing all the merged sections from the RTOS components.
        It is typically obtained via RtosSkeleton.get_module_configuration().

        """
        assert isinstance(name, str)
        assert isinstance(arch, Architecture)
        assert isinstance(sections, dict)
        self._name = name
        self._arch = arch
        self._sections = sections
        self._python_file = python_file

    @property
    def _module_name(self):
        return 'rtos-' + self._name

    @property
    def _module_dir(self):
        module_dir = base_path('packages', self._arch.name, self._module_name)
        os.makedirs(module_dir, exist_ok=True)
        return module_dir

    def generate(self):
        """Generate the RTOS module to disk, so it is available as a compile and link unit to projects."""
        self._render()

    def _render(self):
        python_output = os.path.join(self._module_dir, 'entity.py')
        source_output = os.path.join(self._module_dir, self._module_name + '.c')
        header_output = os.path.join(self._module_dir, self._module_name + '.h')
        config_output = os.path.join(self._module_dir, 'schema.xml')

        source_sections = ['headers', 'object_like_macros',
                           'type_definitions', 'structure_definitions',
                           'extern_definitions', 'function_definitions',
                           'state', 'function_like_macros',
                           'functions', 'public_functions']
        header_sections = ['public_headers', 'public_type_definitions',
                           'public_object_like_macros', 'public_function_like_macros',
                           'public_extern_definitions', 'public_function_definitions']
        sections = self._sections

        with open(source_output, 'w') as f:
            for ss in source_sections:
                data = '\n'.join(sections[ss])
                if ss == 'type_definitions':
                    data = sort_typedefs(data)
                f.write(data)
                f.write('\n')

        with open(header_output, 'w') as f:
            mod_name = self._module_name.upper().replace('-', '_')
            f.write("#ifndef {}_H\n".format(mod_name))
            f.write("#define {}_H\n".format(mod_name))
            for ss in header_sections:
                for data in sections[ss]:
                    f.write(data)
                    f.write('\n')
            f.write("\n#endif /* {}_H */".format(mod_name))

        with open(config_output, 'w') as f:
            f.write('''<?xml version="1.0" encoding="UTF-8" ?>
''')
            schema = merge_schema_sections(sections.get('schema', []))
            f.write(schema)

        shutil.copyfile(self._python_file, python_output)


def generate_rtos_module(skeleton, architectures):
    """Generate RTOS modules for several architectures from a given skeleton."""
    for arch in architectures:
        rtos_module = skeleton.create_configured_module(arch)
        rtos_module.generate()


@contextmanager
def tarfile_open(name, mode, **kwargs):
    assert mode.startswith('w')
    with tarfile.open(name, mode, **kwargs) as f:
        try:
            yield f
        except:
            os.unlink(name)
            raise


class FileWithLicense:
    """FileWithLicense provides a read-only file-like object that automatically includes license text when reading
    from the underlying file object.

    The FileWithLicense object takes ownership of the underlying file object.
    The original file object should not be used after passing it to the FileWithLicense object.

    """
    def __init__(self, f, lic, xml_mode):
        XML_PROLOG = b'<?xml version="1.0" encoding="UTF-8" ?>\n'
        self._f = f
        self._read_license = True

        if xml_mode:
            lic = XML_PROLOG + lic
            file_header = f.read(len(XML_PROLOG))
            if file_header != XML_PROLOG:
                raise Exception("XML File: '{}' does not contain expected prolog: {} expected {}".
                                format(f.name, file_header, XML_PROLOG))

        if len(lic) > 0:
            self._read_license = False
            self._license_io = io.BytesIO(lic)

    def read(self, size):
        assert size > 0
        data = b''
        if not self._read_license:
            data = self._license_io.read(size)
            if len(data) < size:
                self._read_license = True
                size -= len(data)

        if self._read_license:
            data += self._f.read(size)

        return data

    def close(self):
        self._f.close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()


class LicenseOpener:
    """The license opener provides a single 'open' method, that can be used instead of the built-in 'open' function.

    This open will return a file-like object that modifies the underlying file to include an appropriate license
    header.

    The 'license' is passed to the object during construction.

    """
    def __init__(self, license):
        self.license = license

    def _get_lic(self, filename):
        lic = ''
        ext = os.path.splitext(filename)[1]
        is_xml = False

        if ext in ['.c', '.h', '.ld', '.s']:
            lic = '/*' + self.license + '*/\n'
        elif ext in ['.py']:
            lic = '"""' + self.license + '"""\n'
        elif ext in ['.prx']:
            lic = '<!--' + self.license + '-->\n'
            is_xml = True
        elif ext in ['.asm']:
            lic = "\n".join(['; ' + line for line in self.license.rsplit("\n")]) + "\n"

        lic = lic.encode('utf8')

        return lic, is_xml

    def open(self, filename, mode):
        assert mode == 'rb'

        f = open(filename, mode)
        lic, is_xml = self._get_lic(filename)
        return FileWithLicense(f, lic, is_xml)

    def tar_info_filter(self, tarinfo):
        if tarinfo.isreg():
            lic, _ = self._get_lic(tarinfo.name)
            tarinfo.size += len(lic)
        return tar_info_filter(tarinfo)


def tar_info_filter(tarinfo):
    tarinfo.uname = '_default_user_'
    tarinfo.gname = '_default_group_'
    tarinfo.mtime = BASE_TIME
    tarinfo.uid = 1000
    tarinfo.gid = 1000
    return tarinfo


def tar_add_data(tf, arcname, data, ti_filter=None):
    """Directly add data to a tarfile.

    tf is a tarfile.TarFile object.
    arcname is the name the data will have in the archive.
    data is the raw data (which should be of type 'bytes').
    fi_filter filters the created TarInfo object. (In a similar manner to the tarfile.TarFile.add() method.

    """
    ti = tarfile.TarInfo(arcname)
    ti.size = len(data)
    if ti_filter:
        ti = ti_filter(ti)
    tf.addfile(ti, io.BytesIO(data))


def tar_gz_with_license(output, tree, prefix, license):

    """Create a tar.gz file named `output` from a specified directory tree.

    Any appropriate files have the specified license attached.

    When creating the tar.gz a standard set of meta-data will be used to help ensure things are consistent.

    """
    lo = LicenseOpener(license)
    try:
        with tarfile.open(output, 'w:gz', format=tarfile.GNU_FORMAT) as tf:
            tarfile.bltn_open = lo.open
            with chdir(tree):
                for f in os.listdir('.'):
                    tf.add(f, arcname='{}/{}'.format(prefix, f), filter=lo.tar_info_filter)
    finally:
        tarfile.bltn_open = open


class Package:
    """Represents a customer visible package.

    This is currently used mainly for release management.

    """
    @staticmethod
    def create_from_disk():
        """Return a dictionary that contains a Package instance for each package on disk in a 'package' directory.

        The dictionary keys are the package names.
        The dictionary values are the package instances.

        """
        pkgs = {}
        for pkg_parent_dir in base_to_top_paths('packages'):
            pkg_names = os.listdir(pkg_parent_dir)
            for pkg_name in pkg_names:
                pkg_path = os.path.join(pkg_parent_dir, pkg_name)
                if pkg_name in pkgs:
                    logging.warn('Overriding package {} with package {}'.format(pkgs[pkg_name].path, pkg_path))
                pkgs[pkg_name] = Package(pkg_path)
        return pkgs

    def __init__(self, path):
        assert os.path.isdir(path)
        self.path = path
        self.name = os.path.basename(self.path)


class ReleasePackage:
    """Represents a Package instance that is refined for a specific release configuration.

    Configuring a Package instance for release results in additional properties of a package, relevant when creating
    release files.

    """
    def __init__(self, package, release_configuration):
        self._pkg = package
        self._rls_cfg = release_configuration

    def get_name(self):
        return self._pkg.name

    def get_path(self):
        return self._pkg.path

    def get_archive_name(self):
        return '{}-{}'.format(self._pkg.name, self._rls_cfg.release_name)

    def get_path_in_archive(self):
        return 'share/packages/{}'.format(self._pkg.name)

    def get_license(self):
        return self._rls_cfg.license


def mk_partial(pkg):
    fn = top_path('release', 'partials', '{}.tar.gz'.format(pkg.get_archive_name()))
    src_prefix = 'share/packages/{}'.format(pkg.get_name())
    tar_gz_with_license(fn, pkg.get_path(), src_prefix, pkg.get_license())


def build_partials(args):
    build([])
    os.makedirs(top_path('release', 'partials'),  exist_ok=True)
    packages = Package.create_from_disk().values()
    for pkg in packages:
        for config in get_release_configs():
            release_package = ReleasePackage(pkg, config)
            mk_partial(release_package)


def build_manual(pkg):
    manual_file = os.path.join(BASE_DIR, 'packages', pkg, '{}-manual'.format(pkg))
    if not os.path.exists(manual_file):
        print("Manual '{}' does not exist.".format(manual_file))
    else:
        print("Transforming manual '{}'".format(manual_file))


def build_manuals(args):
    build([])
    packages = os.listdir(os.path.join(BASE_DIR, 'packages'))
    for pkg in packages:
        build_manual(pkg)


class ReleaseMeta(type):
    """A pretty-printing meta-class for the Release class."""
    def __str__(cls):
        return '{}-{}'.format(cls.release_name, cls.version)


class Release(metaclass=ReleaseMeta):
    """The Release base class is used by the release configuration."""
    packages = []
    platforms = []
    version = None
    product_name = None
    release_name = None
    enabled = False
    license = None
    top_level_license = None
    extra_files = []


def get_release_configs():
    """Return a list of release configs."""
    import release_cfg
    maybe_configs = [getattr(release_cfg, cfg) for cfg in dir(release_cfg)]
    configs = [cfg for cfg in maybe_configs if inspect.isclass(cfg) and issubclass(cfg, Release)]
    enabled_configs = [cfg for cfg in configs if cfg.enabled]
    return enabled_configs


def build_single_release(config):
    """Build a release archive for a specific release configuration."""
    basename = '{}-{}-{}'.format(config.product_name, config.release_name, config.version)
    logging.info("Building {}".format(basename))
    with tarfile_open(top_path('release', '{}.tar.gz'.format(basename)), 'w:gz', format=tarfile.GNU_FORMAT) as tf:
        for pkg in config.packages:
            release_file_path = top_path('release', 'partials', '{}-{}.tar.gz'.format(pkg, config.release_name))
            with tarfile.open(release_file_path, 'r:gz') as in_f:
                for m in in_f.getmembers():
                    m_f = in_f.extractfile(m)
                    m.name = basename + '/' + m.name
                    tf.addfile(m, m_f)
        for plat in config.platforms:
            arcname = '{}/{}/bin/prj'.format(basename, plat)
            tf.add('prj_build_{}/prj'.format(plat), arcname=arcname, filter=tar_info_filter)
        if config.top_level_license is not None:
            tar_add_data(tf, '{}/LICENSE'.format(basename),
                         config.top_level_license.encode('utf8'),
                         tar_info_filter)

        for arcname, filename in config.extra_files:
            tf.add(filename, arcname='{}/{}'.format(basename, arcname), filter=tar_info_filter)

        if 'TEAMCITY_VERSION' in os.environ:
            build_info = os.environ['BUILD_VCS_NUMBER']
        else:
            g = Git()
            build_info = g.branch_hash()
            if not g.working_dir_clean():
                build_info += "-unclean"
        build_info += '\n'
        tar_add_data(tf, '{}/build_info'.format(basename), build_info.encode('utf8'), tar_info_filter)


def build_release(args):
    """Implement the build-release command.

    Build release takes the various partial releases, and combines them in to a single tar file.

    Additionally, it takes the binary 'prj' files and adds it to the appropriate place in the release tar file.

    """
    for config in get_release_configs():
        try:
            build_single_release(config)
        except FileNotFoundError as e:
            logging.warning("Unable to build '{}'. File not found: '{}'".format(config, e.filename))


def release_test_one(archive):
    """Test a single archive

    This is primarily a sanity check of the actual release file. Release
    files are only made after other testing has been successfully performed.

    Currently it simply does some sanitfy checking on the tar file to ensure it is consistent.

    In the future additional testing will be performed.

    """
    project_prj_template = """<?xml version="1.0" encoding="UTF-8" ?>
<project>
<search-path>{}</search-path>
</project>
"""

    rel_file = os.path.abspath(archive)
    with tarfile.open(rel_file, 'r:gz') as tf:
        for m in tf.getmembers():
            if m.gid != 1000:
                raise Exception("m.gid != 1000 {} -- {}".format(m.gid, m.name))
            if m.uid != 1000:
                raise Exception("m.uid != 1000 {} -- {}".format(m.uid, m.name))
            if m.mtime != BASE_TIME:
                raise Exception("m.gid != BASE_TIME({}) {} -- {}".format(m.mtime, BASE_TIME, m.name))

    platform = get_host_platform_name()

    with tempdir() as td:
        with chdir(td):
            assert shutil.which('tar')
            subprocess.check_call("tar xf {}".format(rel_file).split())
            release_dir = os.path.splitext(os.path.splitext(os.path.basename(archive))[0])[0]
            if not os.path.isdir(release_dir):
                raise RuntimeError("Release archive does not extract into top directory with the same name as the "
                                   "base name of the archive ({})".format(release_dir))
            with chdir(release_dir):
                cmd = "./{}/bin/prj".format(platform)
                try:
                    subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
                except subprocess.CalledProcessError as e:
                    if e.returncode != 1:
                        raise e
                pkgs = []
                pkg_root = './share/packages/'
                for root, _dir, files in os.walk(pkg_root):
                    for f in files:
                        if f.endswith('.prx'):
                            pkg = os.path.join(root, f)[len(pkg_root):-4].replace(os.sep, '.')
                            pkgs.append((pkg, os.path.join(root, f)))
                with open('project.prj', 'w') as f:
                    f.write(project_prj_template.format(pkg_root))
                for pkg, f in pkgs:
                    cmd = "./{}/bin/prj build {}".format(platform, pkg)
                    try:
                        subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
                    except subprocess.CalledProcessError as e:
                        err_str = None
                        for l in e.output.splitlines():
                            l = l.decode()
                            if l.startswith('ERROR'):
                                err_str = l
                                break
                        if err_str is None:
                            print(e.output)
                            raise e
                        elif 'missing or contains multiple Builder modules' in err_str:
                            pass
                        else:
                            print("Unexpected error:", err_str)
                            raise e


def release_test(args):
    """Implement the test-release command.

    This command is used to perform sanity checks and testing of the full release.

    """
    for rel in glob(top_path('release', '*.tar.gz')):
        release_test_one(rel)


class OverrideFunctor:
    def __init__(self, function, *args, **kwargs):
        self.function = function
        self.args = args
        self.kwargs = kwargs

    def __call__(self, *args, **kwargs):
        return self.function(*self.args, **self.kwargs)


CORE_ARCHITECTURES = {
    'posix': Architecture('posix', {}),
    'armv7m': Architecture('armv7m', {}),
}

CORE_SKELETONS = {
    'sched-rr-test': RtosSkeleton(
        'sched-rr-test',
        [Component('reentrant'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('sched-rr-test')]),
    'sched-prio-test': RtosSkeleton(
        'sched-prio-test',
        [Component('reentrant'),
         Component('sched', 'sched-prio', {'assume_runnable': False}),
         Component('sched-prio-test')]),
    'sched-prio-inherit-test': RtosSkeleton(
        'sched-prio-inherit-test',
        [Component('reentrant'),
         Component('sched', 'sched-prio-inherit', {'assume_runnable': False}),
         Component('sched-prio-inherit-test')]),
    'simple-mutex-test': RtosSkeleton(
        'simple-mutex-test',
        [Component('reentrant'),
         Component('mutex', 'simple-mutex'),
         Component('simple-mutex-test')]),
    'blocking-mutex-test': RtosSkeleton(
        'blocking-mutex-test',
        [Component('reentrant'),
         Component('mutex', 'blocking-mutex'),
         Component('blocking-mutex-test')]),
    'simple-semaphore-test': RtosSkeleton(
        'simple-semaphore-test',
        [Component('reentrant'),
         Component('semaphore', 'simple-semaphore'),
         Component('simple-semaphore-test')]),
    'acamar': RtosSkeleton(
        'acamar',
        [Component('reentrant'),
         Component('acamar'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('error'),
         Component('task'),
         ]),
    'gatria': RtosSkeleton(
        'gatria',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('gatria'),
         ]),
    'kraz': RtosSkeleton(
        'kraz',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('signal'),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('kraz'),
         ]),
    'acrux': RtosSkeleton(
        'acrux',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         ArchitectureComponent('interrupt_event_arch', 'interrupt-event'),
         Component('interrupt_event', 'interrupt-event', {'timer_process': False, 'task_set': False}),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('acrux'),
         ]),
    'rigel': RtosSkeleton(
        'rigel',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('signal'),
         ArchitectureComponent('timer_arch', 'timer'),
         Component('timer'),
         ArchitectureComponent('interrupt_event_arch', 'interrupt-event'),
         Component('interrupt_event', 'interrupt-event', {'timer_process': True, 'task_set': True}),
         Component('mutex', 'blocking-mutex'),
         Component('profiling'),
         Component('message-queue'),
         Component('error'),
         Component('task'),
         Component('rigel'),
         ],
    ),
}


CORE_CONFIGURATIONS = {
    'sched-rr-test': ['posix'],
    'sched-prio-inherit-test': ['posix'],
    'simple-mutex-test': ['posix'],
    'blocking-mutex-test': ['posix'],
    'simple-semaphore-test': ['posix'],
    'sched-prio-test': ['posix'],
    'acamar': ['posix', 'armv7m'],
    'gatria': ['posix', 'armv7m'],
    'kraz': ['posix', 'armv7m'],
    'acrux': ['armv7m'],
    'rigel': ['armv7m'],
}


# client repositories may extend or override the following variables to control which configurations are available
architectures = CORE_ARCHITECTURES.copy()
skeletons = CORE_SKELETONS.copy()
configurations = CORE_CONFIGURATIONS.copy()


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    SUBCOMMAND_TABLE = {
        'prj-build': prj_build,
        'build': build,
        'test-release': release_test,
        'build-release': build_release,
        'build-partials': build_partials,
        'build-manuals': build_manuals,
        # Testing
        'check-pep8': check_pep8,
        'prj-test': prj_test,
        'pystache-test': pystache_test,
        'x-test': x_test,
        'rtos-test': rtos_test,
        # Tasks management
        'new-review': new_review,
        'new-task': new_task,
        'tasks': tasks,
        'integrate': integrate,
    }

    # create the top-level parser
    parser = argparse.ArgumentParser(prog='x.py')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    # create the parser for the "prj.pep8" command
    subparsers.add_parser('tasks', help="List tasks")

    _parser = subparsers.add_parser('check-pep8', help='Run PEP8 on project Python files')
    _parser.add_argument('--teamcity', action='store_true',
                         help="Provide teamcity output for tests",
                         default=False)
    _parser.add_argument('--excludes', nargs='*',
                         help="Exclude directories from pep8 checks",
                         default=[])

    for component_name in ['prj', 'x', 'rtos']:
        _parser = subparsers.add_parser(component_name + '-test', help='Run {} unittests'.format(component_name))
        _parser.add_argument('tests', metavar='TEST', nargs='*',
                             help="Specific test", default=[])
        _parser.add_argument('--list', action='store_true',
                             help="List tests (don't execute)",
                             default=False)
        _parser.add_argument('--verbose', action='store_true',
                             help="Verbose output",
                             default=False)
        _parser.add_argument('--quiet', action='store_true',
                             help="Less output",
                             default=False)
    subparsers.add_parser('prj-build', help='Build prj')

    subparsers.add_parser('pystache-test', help='Test pystache')
    subparsers.add_parser('build-release', help='Build final release')
    subparsers.add_parser('test-release', help='Test final release')
    subparsers.add_parser('build-partials', help='Build partial release files')
    subparsers.add_parser('build-manuals', help='Build PDF manuals')
    subparsers.add_parser('build', help='Build all release files')
    _parser = subparsers.add_parser('new-review', help='Create a new review')
    _parser.add_argument('reviewers', metavar='REVIEWER', nargs='+',
                         help='Username of reviewer')

    _parser = subparsers.add_parser('new-task', help='Create a new task')
    _parser.add_argument('taskname', metavar='TASKNAME', help='Name of the new task')
    _parser.add_argument('--no-fetch', dest='fetch', action='store_false', default='true', help='Disable fetchign')

    # generate parsers and command table entries for generating RTOS variants
    for rtos_name, arch_names in configurations.items():
        SUBCOMMAND_TABLE[rtos_name + '-gen'] = OverrideFunctor(generate_rtos_module,
                                                               skeletons[rtos_name],
                                                               [architectures[arch] for arch in arch_names])
        subparsers.add_parser(rtos_name + '-gen', help="Generate {} RTOS".format(rtos_name))

    _parser = subparsers.add_parser('integrate', help='Integrate a completed development task/branch into the main \
upstream branch.')
    _parser.add_argument('--repo', help='Path of git repository to operate in. \
Defaults to current working directory.')
    _parser.add_argument('--name', help='Name of the task branch to integrate. \
Defaults to active branch in repository.')
    _parser.add_argument('--target', help='Name of branch to integrate task branch into. \
Defaults to "development".', default='development')
    _parser.add_argument('--archive', help='Prefix to add to task branch name when archiving it. \
Defaults to "archive".', default='archive')

    args = parser.parse_args()

    # Default to building
    if args.command is None:
        args.command = 'build'
    args.topdir = topdir

    return SUBCOMMAND_TABLE[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
