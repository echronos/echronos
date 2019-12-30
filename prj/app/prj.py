#!/usr/bin/env python3
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

"""Firmware system project management tool.

Main entry point.

"""
from distutils.spawn import find_executable
from xml.parsers.expat import ExpatError
import argparse
import functools
import imp
import inspect
import os
import pdb
import shutil
import signal
import subprocess
import sys
import traceback

if __name__ == "__main__":
    for pth in ['../../external_tools', 'lib']:
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), pth))

# Logging is set up first since this is critical to the rest of the application working correctly.
# It is possible that other modules will perform logging during import, so make sure this is very early.
# pylint: disable=wrong-import-position
import logging as _logging  # Avoid unintended using of 'logging'
# By default log everything from INFO up.
logger = _logging.getLogger('prj')  # pylint: disable=invalid-name
logger.setLevel(_logging.INFO)
_logging.basicConfig()


# Ensure that basicConfig is only called once.
def error_fn(*_, **__):
    raise Exception("basicConfig called multiple times.")
_logging.basicConfig = error_fn

import pystache.parser
import pystache.renderer
from util.util import prepend_tool_binaries_to_path_environment_variable
from util.xml import UserError, NOTHING, xml_parse_file, single_text_child, maybe_single_named_child,\
    xml_parse_file_with_includes, xml_parse_string, get_attribute, single_named_child, xml2schema,\
    xml2dict, SystemParseError, xml_error_str, maybe_get_element_list, check_schema_is_valid, SchemaInvalidError

# Configure the pystache module
pystache.defaults.MISSING_TAGS = 'strict'

# Place this module in to the modules namespace as 'prj' which enables
# any plug-in modules to "import prj". This may be done differently
# in the future as some of this script is split in to different
# files.
sys.modules['prj'] = sys.modules[__name__]


SIG_NAMES = dict((k, v) for v, k in signal.__dict__.items() if v.startswith('SIG'))


def canonical_path(path):
    return os.path.normcase(os.path.normpath(os.path.realpath(os.path.abspath(path))))


def canonical_paths(paths):
    return [canonical_path(path) for path in paths]


def follow_link(link):
    """Return the underlying file form a symbolic link.

    This will (recursively) follow links until a non-symbolic link is found.
    No cycle checks are performed by this function.

    """
    if not os.path.islink(link):
        return link
    path = os.readlink(link)
    if os.path.isabs(path):
        return path
    return follow_link(os.path.join(os.path.dirname(link), path))


def show_exit(exit_code):
    sig_num = exit_code & 0xff
    exit_status = exit_code >> 8
    if sig_num == 0:
        return "exit: {}".format(exit_status)
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
    renderer.register_formatter('u', lambda x: x.upper())
    # Note: This can only be used on integer values
    renderer.register_formatter('hex', lambda x: str(hex(int(x))))

    with open(file_in, 'r') as inp:
        template_data = inp.read()

    try:
        parsed_template = pystache.parser.parse(template_data, name=file_in)
        data = renderer.render(parsed_template, config)
    except pystache.common.PystacheError as exc:
        raise SystemBuildError("Error rendering template '{}'. {}.".format(exc.location, str(exc)))

    os.makedirs(os.path.dirname(file_out), exist_ok=True)

    with open(file_out, 'wb') as outp:
        outp.write(data.encode())


# We don't want byte-code written to disk for any of the plug-ins that we load,
# so disable it here. It would be nicer to do this on a per-plugin basis but
# that doesn't appear possible from the Python source
sys.dont_write_bytecode = True


class ResourceNotFoundError(UserError):
    """Raised when the system is unable to find a file or other resource.

    This is very similar to the builtin FileNotFoundError however, this
    allows us customisation of the error message.
    """


class SystemConsistencyError(UserError):
    """Indicates that the system, as instantiated from its system definition, is internally inconsistent.
    For example, a required module may be missing.

    """


class SystemBuildError(UserError):
    """Raised when an error occurs during system build."""


class SystemLoadError(UserError):
    """Raised when an error occurs while loading and starting a system image on a device."""


class EntityLoadError(Exception):
    """Raise when unable to resolve a entity reference."""
    def __init__(self, msg, detail=None):
        super().__init__(msg)
        self.detail = detail


class EntityNotFoundError(UserError):
    """Raised when an entity can not be found."""


class ProjectError(Exception):
    """Raised when there is an error constructing the Project class."""


class ProjectStartupError(Exception):
    """Raised when there is an error initialising the start-script."""


def valid_entity_name(name):
    """Return true if the name is a valid entity name.

    Entity names can not contain forward slash or backslash characters.

    """
    return not any([bad_char in name for bad_char in '/\\'])


def execute(args, **kwargs):
    """Execute a command.

    This wraps sucprocess.call in appropriate logging calls to display the command being executed, and raising an
    exception in the case of an error.

    Optional additional keyword arguments are passed verbatim to subprocess.call().

    """
    cmd_line = ' '.join(args)
    logger.info('Executing: %s', cmd_line)
    try:
        code = subprocess.call(args, **kwargs)
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
        self.module = module
        self._system = system

    def __repr__(self):
        return '<{} {} of module {}>'.format(self.__class__.__name__, id(self), repr(self.module))

    def __getattr__(self, name):
        """ModuleInstances do not have methods, however they provide shortcuts to the underlying Module's methods.

        `instance.<foo>()` is a short-cut for `instance.module.<foo>(instance._system, instance._config)`

        """
        try:
            func = getattr(self.module, name)
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

    Any sub-class may override all methods to provide customisation, however in many cases this is not necessary, and
    simply setting some class level variables is sufficient.

    The `schema` class variable is the schema used when parsing the user's configuration.
    The `xml_schema` class variable is an XML representation of the schema.
    Only one of `schema` and `xml_schema` should be set.

    The `files` class variable is a list of dictionaries representing files that should be installed during the
    prepare or post-prepare stages.
    Each dictionary has the following fields:
       'input': The input file-name.
       'output': The output file-name (optional, defaults to the input filename).
       'render': Whether pystache rendering should be applied to the file (defaults to False).
       'type': The type of the file (if any).
               'c': The file should be added as a C file to the system.
               'asm': The file should be added as a assembler file to the system.
               'linker_script': The file should be added as the linker script in the system.
       'stage': The stage at which the file should be prepared: `prepare` or `post_prepare`. Defaults to `prepare`.

    """
    schema = NOTHING
    xml_schema = NOTHING
    xml_schema_path = NOTHING
    files = NOTHING

    def __init__(self):
        """Initialise the module.

        Module initialisation will ensures the class is correctly constructed.
        If the xml_schema is set, initialisation will set the schema based on the xml_schema.

        """
        self.name = None

        if len(set([self.schema, self.xml_schema, self.xml_schema_path])) > 2:
            raise Exception("Class '{}' in {} has multiple schema sources set."
                            .format(self.__class__.__name__, os.path.abspath(inspect.getfile(self.__class__))))

        if self.schema is NOTHING:
            if self.xml_schema_path is not NOTHING:
                xml_schema_document = xml_parse_file(self.xml_schema_path)
            elif self.xml_schema is not NOTHING:
                filename = sys.modules[self.__class__.__module__].__file__
                xml_schema_document = xml_parse_string(self.xml_schema, '{}!xml_schema'.format(filename))
            else:
                raise Exception("Class '{}' in {} has none of the possible schema sources (schema, xml_schema, \
xml_schema_path) set as a class member.".format(self.__class__.__name__,
                                                os.path.abspath(inspect.getfile(self.__class__))))
            self.schema = xml2schema(xml_schema_document)

    def configure(self, xml_config):
        """Configure a module.

        `xml_config` is an XML element describing the configuration of the module.
        The method should return an object, which will later be passed to `prepare` and `validate`.
        This method can raise exceptions if the `xml_config` is invalid.

        """
        if self.schema is NOTHING:
            return None
        return xml2dict(xml_config, self.schema)

    def validate(self, system, config):
        """Validate that the `config` is correct within the context of the given `system`.

        This would include such things as dependencies on other modules.
        This is called after all the system's module instances are created, so as to allow for the possibility of
        mutual dependencies or other advanced checks.

        """
        pass

    def _prepare_files(self, system, config, stage):
        """Prepare the files in the objects's `files` variable for building.

        """
        if self.files is NOTHING:
            return

        # Input file names are related to the module's path.
        module_path = sys.modules[self.__class__.__module__].__path__

        for file_obj in self.files:  # pylint: disable=not-an-iterable
            if file_obj.get('stage', 'prepare') != stage:
                continue

            input_path = os.path.join(module_path, file_obj['input'])
            if 'output' in file_obj:
                output_path = os.path.join(system.output, file_obj['output'])
            else:
                output_path = system.get_output_path_for_file(file_obj['input'], self.name)

            _prepare_template(input_path, output_path, file_obj.get('render', False), config)

            _type = file_obj.get('type')
            if _type is None:
                # make headers discoverable by the compiler
                if os.path.splitext(file_obj['input'])[1].lower() == '.h':
                    system.add_include_path(os.path.dirname(output_path))
            elif _type == 'c':
                system.add_c_file(output_path)
            elif _type == 'asm':
                system.add_asm_file(output_path)
            elif _type == 'linker_script':
                system.linker_script = output_path
            else:
                raise Exception("Unexpected type '{}' for file '{}'".format(_type, file_obj['input']))

    def prepare(self, system, config, **_):
        """Prepare the `system` for building based on a specific module `config`.

        This method should be implemented in a sub-class. It should update the system object
        making it ready to be passed to a Builder module. Additionally it may update the
        filesystem to generate files.

        """
        self._prepare_files(system, config, stage="prepare")

    def post_prepare(self, system, config):
        self._prepare_files(system, config, stage="post_prepare")

    def __repr__(self):
        return '<{}>'.format(cls_name(self.__class__))


def _prepare_template(input_path, output_path, render, config):
    logger.info("Preparing: template %s -> %s", input_path, output_path)
    try:
        if render:
            pystache_render(input_path, output_path, config)
        else:
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            shutil.copyfile(input_path, output_path)
    except FileNotFoundError as exc:
        raise SystemBuildError("File not found error during template preparation '{}'.".format(exc.filename))


class NamedModule(Module):
    # pylint: disable=super-init-not-called
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return '<{} name="{}">'.format(cls_name(self.__class__), self.name)


class SourceModule(NamedModule):
    # pylint: disable=too-many-branches
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
        extensions = ['.c', '.s']
        if any([filename.endswith(ext) for ext in extensions]):
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
            return os.path.join(os.path.dirname(self.filename), hdr)

        for hdr_el in hdr_els:
            code_gen = hdr_el.getAttributeNode('code_gen')
            code_gen = code_gen.value if code_gen is not None else None
            if code_gen not in [None, 'template']:
                raise SystemParseError(xml_error_str(hdr_el, "not a supported code_gen type: %s" % code_gen))
            self.headers.append(Header(fix_path(get_attribute(hdr_el, "path")), code_gen, hdr_el))

        schema = maybe_single_named_child(dom, 'schema')
        self.schema = xml2schema(schema) if schema else None

    def prepare(self, system, config, *, copy_all_files=False):  # pylint: disable=arguments-differ
        """prepare the `system` for building based on the specific config.

        This includes copying any header files to the correct locations, and running any templating.
        All modules are only prepared after validation of all modules has occurred.
        Compilation does not occur until all modules are prepared.

        This is currently only stubbed.

        """
        if self.code_gen is None:
            if copy_all_files:
                path = system.get_output_path_for_file(self.filename, self.name)
                os.makedirs(os.path.dirname(path), exist_ok=True)
                shutil.copyfile(self.filename, path)
                logger.info("Preparing: copy %s -> %s", self.filename, path)
                system.add_file(path)
            else:
                system.add_file(self.filename)

        elif self.code_gen == 'template':
            # Create implementation file.
            path = system.get_output_path_for_file(self.filename, self.name)
            logger.info("Preparing: template %s -> %s (%s)", self.filename, path, config)
            pystache_render(self.filename, path, config)
            system.add_file(path)

        # Copy any headers across. This should use templating if that is configured.
        for header in self.headers:
            path = system.get_output_path_for_file(header.path, self.name)
            os.makedirs(os.path.dirname(path), exist_ok=True)
            try:
                if header.code_gen is None:
                    shutil.copyfile(header.path, path)
                elif header.code_gen == 'template':
                    logger.info("Preparing: template %s -> %s (%s)", header.path, path, config)
                    pystache_render(header.path, path, config)
                system.add_include_path(os.path.dirname(path))
            except FileNotFoundError:
                error_str = xml_error_str(header.xml_element, "Resource not found: {}".format(header.path))
                raise ResourceNotFoundError(error_str)


class Action(NamedModule):
    def __init__(self, name, py_module):
        super().__init__(name)
        assert hasattr(py_module, 'run')
        self._py_module = py_module
        if hasattr(py_module, 'schema'):
            try:
                check_schema_is_valid(py_module.schema)
            except SchemaInvalidError as exc:
                raise SystemLoadError("The schema declared in module '{}' is invalid. {:s}".format(name, exc))
            self.schema = py_module.schema

    def run(self, system, config):
        try:
            self._py_module.run(system, config)
        except (SystemBuildError, ):
            raise
        except Exception:
            file_name = '{}.errors.log'.format(self.name)
            with open(file_name, "w") as file_obj:
                traceback.print_exception(*sys.exc_info(), file=file_obj)
            msg_fmt = "An unexpected error occured while running custom module '{}'. Traceback saved to '{}'."
            raise SystemBuildError(msg_fmt.format(self.name, file_name))


class Builder(Action):
    pass


class Loader(Action):
    pass


# pylint: disable=too-many-instance-attributes
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
        self._include_paths = []
        self._output = None
        self.__instances = None
        self._output_names = {}

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
        return [self.output] + self._include_paths

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

    def add_include_path(self, path):
        self._include_paths.append(path)

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

    # pylint: disable=too-many-locals
    # pylint: disable=too-many-branches
    def _get_instances(self):
        """Instantiate all modules referenced in the system definition file of this system and validate them.
        This is a prerequisite to invoking such operations as build or load on a system.

        Returns a list of instances of class ModuleInstance.

        """
        self._parse_include_paths()

        # Parse the DOM to load all the entities.
        module_el = single_named_child(self.dom, 'modules')
        module_els = [elem for elem in module_el.childNodes
                      if elem.nodeType == elem.ELEMENT_NODE and elem.tagName == 'module']

        instances = []
        rtos_config_data = {}
        rtos_module_name = None
        non_rtos_modules = []

        for m_el in module_els:
            # First find the module
            name = get_attribute(m_el, 'name')
            if not valid_entity_name(name):
                raise EntityLoadError(xml_error_str(m_el, "Invalid module name '{}'".format(name)))

            module = self.project.find(name)

            if isinstance(module, Module):
                try:
                    config_data = module.configure(m_el)
                except SystemParseError:
                    # The module's configure module is allowed to raise a SystemParseError
                    # we just re-raise it.
                    raise
                except Exception:
                    exc_type, exc_value, trace = sys.exc_info()
                    tb_str = ''.join(traceback.format_exception(exc_type, exc_value, trace.tb_next, chain=False))
                    msg = "Error running module '{}' configure method.".format(name)
                    detail = "Traceback:\n{}".format(tb_str)
                    raise EntityLoadError(msg, detail)

                # Find the config data of the RTOS module, identified by a reserved substring
                if ".rtos-" in name:
                    if rtos_module_name is not None:
                        raise EntityLoadError(xml_error_str(m_el, "Multiple RTOS modules found, '{}' and '{}'".format(
                            rtos_module_name, name)))
                    if not config_data:
                        raise EntityLoadError(xml_error_str(m_el, "RTOS module '{}' has no config data".format(name)))

                    # Commit the RTOS module alone
                    instance = ModuleInstance(module, self, config_data)
                    instances.append(instance)

                    rtos_config_data = config_data
                    rtos_module_name = name
                else:
                    non_rtos_modules.append((name, module, config_data, m_el))
            else:
                raise EntityLoadError(xml_error_str(m_el, 'Entity {} has unexpected type {} and cannot be \
                instantiated'.format(name, type(module))))

        # Commit each non-RTOS module's config, with that of the RTOS module present as a dict under the key 'rtos'
        for (name, module, config_data, m_el) in non_rtos_modules:
            if not config_data:
                config_data = {}
            elif 'rtos' in config_data.keys():
                raise EntityLoadError(xml_error_str(m_el, "Module '{}' cannot have a configuration item with the "
                                                          "reserved name 'rtos'.").format(name))
            config_data['rtos'] = rtos_config_data
            instance = ModuleInstance(module, self, config_data)
            instances.append(instance)

        os.makedirs(self.output, exist_ok=True)

        for i in instances:
            i.validate()

        return instances

    def _parse_include_paths(self):
        # Parse the DOM to load any additional include paths
        include_el = maybe_single_named_child(self.dom, 'include_paths')

        if include_el is None:
            return

        # Find all include elements, ignoring any that are empty
        include_els = [elem for elem in include_el.childNodes
                       if elem.nodeType == elem.ELEMENT_NODE and elem.tagName == 'include_path' and elem.firstChild]

        for i_el in include_els:
            path = i_el.firstChild.nodeValue

            # If we aren't given an absolute path treat it as relative the base path
            if not os.path.isabs(path):
                inc_path = os.path.join(os.getcwd(), path)

                # Absolute paths make it very obvious what directory prj is actually including
                path = os.path.abspath(inc_path)

            path = os.path.normpath(path)
            self.add_include_path(path)
            logger.info("Added include path: %s", path)

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

    def analyze(self):
        try:
            subprocess.check_output(["splint", "--help"], stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError:
            print("Unable to invoke 'splint' command. The splint static code analysis tool might not be installed or \
might not be available on the PATH search path for executables.")
            return 1

        self.generate(copy_all_files=False)

        options = ["+quiet"]
        include_paths = self.include_paths

        if sys.platform in ("win32", "cygwin") and "cygwin" not in find_executable("splint"):
            options += ["-nolib", "-booltype", "bool", "+charint"]
            prx_path = self.project.entity_name_to_path(self.name)
            prx_dir = os.path.dirname(prx_path)
            include_paths += [os.path.join(prx_dir, "include")]
        else:
            options += ["-DUINT8_C(x)=(uint8_t)(x)", "-DUINT8_MAX=255",
                        "-DUINT32_C(x)=(uint32_t)(x)", "-DUINT32_MAX=0xFFFFFFFF"]

        options += ['-I{}'.format(include_path) for include_path in include_paths]

        for c_file in self.c_files:
            if os.path.basename(c_file).startswith('rtos-'):
                try:
                    # define UINT macros because splint does not pick them up from system headers for unknown reason
                    # +charintliteral to allow code such as 'int value = ascii_character - '0';'
                    cmd = ["splint"] + options + [c_file]
                    subprocess.check_call(cmd)
                except subprocess.CalledProcessError:
                    print("Static analysis of '{}' with splint command {} failed".format(c_file, cmd))
                    return 2

        return 0

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
        return [i for i in self._instances if isinstance(i.module, typ)]

    def get_output_path_for_file(self, file_path, entity_name):
        """Derive (but do not create) a unique path in this system's output directory for the given input file."""
        return os.path.join(self.get_output_path_for_entity(entity_name), os.path.basename(file_path))

    def get_output_path_for_entity(self, entity_name):
        """Derive (but do not create) a path in this system's output directory corresponding to the given entity."""
        return os.path.join(self.output, *entity_name.split('.')[:-1])

    def __str__(self):
        return "System: %s" % self.name


class Project:
    """The Project is a container for other objects in the system."""

    # pylint: disable=too-many-locals
    # pylint: disable=too-many-branches
    def __init__(self, filename, search_paths=None, prx_include_paths=None):
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
        for script_element in ss_els:
            command = single_text_child(script_element)
            if command.split()[0].endswith('.py'):
                # prepend full path of python interpreter as .py files are not necessarily executable on Windows
                # and the command 'python3' is likely not in PATH
                command = '{} {}'.format(sys.executable, command)
            ret_code = os.system(command)
            if ret_code != 0:
                err = xml_error_str(script_element, "Error running startup-script"
                                                    ": '{}' {}".format(command, show_exit(ret_code)))
                raise ProjectStartupError(err)

        param_search_paths = search_paths if search_paths is not None else []
        project_search_paths = list(get_paths_from_dom(self.dom, 'search-path'))
        user_search_paths = param_search_paths + project_search_paths
        if not user_search_paths:
            user_search_paths = [self.project_dir]

        built_in_search_paths = []
        self.search_paths = user_search_paths + built_in_search_paths

        logger.debug("search_paths %s", self.search_paths)

        param_prx_include_paths = prx_include_paths if prx_include_paths is not None else []
        self._prx_include_paths = list(get_paths_from_dom(self.dom, 'prx-include-path')) + param_prx_include_paths

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
        logger.debug("searching %s", entity_name)

        # Search for a given entity name.
        extensions = ['', '.prx', '.py', '.c', '.s', '.asm']

        # Find the first path that exists, we try and load that.  If a
        # path exists, but fails to load for some other reason that is
        # an error, and we don't attempt to load anything else.
        def search_inner(base):
            for ext in extensions:
                path = '%s%s' % (base, ext)
                logger.debug("trying %s", path)
                if os.path.exists(path):
                    logger.debug("found %s @ %s", entity_name, path)
                    return path, ext
            return None, None

        for search_path in self.search_paths:
            base = os.path.join(search_path, *entity_name.split('.'))
            path, ext = search_inner(base)
            if path is not None:
                break

        if path is None:
            raise EntityNotFoundError("Unable to find entity named '{}' in search paths '{}'"
                                      .format(entity_name, self.search_paths))

        if ext == '':
            if not os.path.isdir(path):
                raise EntityNotFoundError("Unable to find entity named %s" % entity_name)
            # Search for an 'entity.<ext>' file.
            file_path, ext = search_inner(os.path.join(path, 'entity'))
            if file_path is None:
                raise EntityNotFoundError("Couldn't find entity definition file in '{}'".format(path))
            path = file_path

        return path

    def parse_import(self, entity_name, path):
        """Parse an entity decribed in the specified path.

        Return the approriate object as determined by the file extension.

        """
        ext = os.path.splitext(path)[1]
        if ext == '.prx':
            os.makedirs(self.output, exist_ok=True)
            try:
                return System(entity_name,
                              xml_parse_file_with_includes(path, self._prx_include_paths,
                                                           os.path.join(self.output, entity_name + ext)),
                              self)
            except ExpatError as exc:
                raise EntityLoadError("Error parsing system import '{}:{}': {!s}".format(exc.path, exc.lineno, exc))
        elif ext == '.py':
            try:
                py_module = imp.load_source("__prj.%s" % entity_name, path)
            except:
                exc_type, exc_value, trace = sys.exc_info()
                tb_str = ''.join(traceback.format_exception(exc_type, exc_value, trace.tb_next, chain=False))
                msg = "An error occured while loading '{}'".format(path)
                detail = "Traceback:\n{}".format(tb_str)
                raise EntityLoadError(msg, detail)

            py_module.__path__ = os.path.dirname(py_module.__file__)
            if hasattr(py_module, 'system_build'):
                return Builder(entity_name, py_module)
            elif hasattr(py_module, 'system_load'):
                return Loader(entity_name, py_module)
            elif hasattr(py_module, 'module'):
                module = py_module.module
                module.name = entity_name
                return module
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
            self.entities[entity_name] = self.parse_import(entity_name, path)

        return self.entities[entity_name]


def get_paths_from_dom(dom, element_name):
    """Return the contents of all elements with a given name as a list of paths.

    `dom` is a DOM element from a parsed XML document.
    `element_name` is a string matched against the names of all elements in `dom`.
    The text node value contained in each such element is added to the return value.
    Trailing directory delimiters are removed from the end of each such string.

    This function returns an iterator over strings.

    """
    for sp_el in dom.getElementsByTagName(element_name):
        yield single_text_child(sp_el).rstrip(os.sep)


def generate(args):
    """Genereate the source code for the specified system.

    `args` is expected to provide the following attributes:
    - `project`: an instance of Project.
    - `system`: the name of the system entity to instantiate and generate source.

    This function returns 0 on success and 1 if an error occurs.

    """
    return call_system_function(args, System.generate, extra_args={'copy_all_files': True})


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


def analyze(args):
    """Statically analyze the code of the system specified on the command line.

    `args` is expected to provide the following attributes:
    - `project`: an instance of Project
    - `system`: the name of a system entity to analyze

    This function returns 0 on success and 1 if an error occurs.

    """
    return call_system_function(args, System.analyze)


def call_system_function(args, function, extra_args=None):
    """Instantiate a system and call the given member function of the System class on it."""
    project = args.project
    system_name = args.system

    if extra_args is None:
        extra_args = {}

    try:
        if not valid_entity_name(system_name):
            logger.error("System name '%s' is invalid.", system_name)
            return 1
        logger.info("Loading system: %s", system_name)
        system = project.find(system_name)
    except EntityLoadError as exc:
        logger.error("Unable to load system [%s]: %s, %s", system_name, exc, exc.detail)
        return 1
    except EntityNotFoundError as exc:
        logger.error("Unable to find system [%s] (%s).", system_name, str(exc))
        return 1

    if args.output:
        system.output = args.output

    prepend_tool_binaries_to_path_environment_variable()

    logger.info("Invoking '%s' on system '%s'", function.__name__, system.name)
    try:
        return function(system, **extra_args)
    except UserError as exc:
        logger.error(str(exc))
        return 1

    return 0


SUBCOMMAND_TABLE = {
    'gen': generate,
    'build': build,
    'load': load,
    'analyze': analyze,
}


def get_command_line_arguments():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='prj')
    parser.add_argument('--project', default=None,
                        help='project file (project.prj)')
    parser.add_argument('--no-project', action='store_true',
                        help='force no project file')
    parser.add_argument('--search-path', action='append', help='additional search paths')
    parser.add_argument('--verbose', action='store_true', help='provide verbose output')
    parser.add_argument('--quiet', action='store_true', help='provide less output')
    parser.add_argument('--output', '-o', help='Output directory')
    parser.add_argument('--prx-inc-path', action='append',
                        help='Search paths for resolving "include" elements in system definition files. '
                             'These paths are appended to the ones specified in the project file.')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    build_parser = subparsers.add_parser('gen', help='Generate source code for a system')
    build_parser.add_argument('system', help='system to generate source for')

    build_parser = subparsers.add_parser('build', help='Build a system and create a system image')
    build_parser.add_argument('system', help='system to build')

    load_parser = subparsers.add_parser('load', help='Load a system image onto a device and execute it')
    load_parser.add_argument('system', help='system to load')

    load_parser = subparsers.add_parser('analyze', help='Statically analyze the code of a system')
    load_parser.add_argument('system', help='system to analyze')

    args = parser.parse_args()

    if args.quiet:
        logger.setLevel(_logging.WARNING)

    if args.verbose:
        logger.setLevel(_logging.DEBUG)

    if args.command is None:
        parser.print_help()
        parser.exit(1, "\nSee 'prj <subcommand> -h' for more information on a specific command\n")

    if not args.no_project and args.project is None:
        args.project = 'project.prj'

    if args.no_project:
        args.project = None

    return args


def report_error(exception):
    logger.error(str(exception))
    if hasattr(exception, 'detail') and exception.detail is not None:
        logger.error(exception.detail)
    return 1


def commonpath(paths):
    if hasattr(os.path, 'commonpath'):
        commonpath_func = getattr(os.path, 'commonpath')
        return commonpath_func(paths)
    else:
        prefix = commonprefix(paths)
        if os.path.exists(prefix):
            return prefix
        return os.path.dirname(prefix)


def commonprefix(paths):
    if hasattr(os.path, 'commonprefix'):
        return os.path.commonprefix(paths)
    else:
        if not paths:
            return ''
        # Some people pass in a list of pathname parts to operate in an OS-agnostic
        # fashion; don't try to translate in that case as that's an abuse of the
        # API and they are already doing what they need to be OS-agnostic and so
        # they most likely won't be using an os.PathLike object in the sublists.
        if not isinstance(paths[0], (list, tuple)):
            raise ValueError("paths[0] has unsupported type {}".format(type(paths[0])))
        str1 = min(paths)
        str2 = max(paths)
        for idx, component in enumerate(str1):
            if component != str2[idx]:
                return str1[:idx]
        return str1


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    args = get_command_line_arguments()

    logger.setLevel(_logging.WARNING)
    if args.verbose:
        logger.setLevel(_logging.INFO)
    elif args.quiet:
        logger.setLevel(_logging.ERROR)

    # Initialise project
    try:
        args.project = Project(args.project, args.search_path, args.prx_inc_path)
    except (EntityLoadError, EntityNotFoundError, ProjectStartupError) as exc:
        return report_error(exc)
    except FileNotFoundError as exc:
        logger.error("Unable to initialise project from file [%s]. Exception: %s", args.project, exc)
        return 1
    except ExpatError as exc:
        logger.error("Parsing %s:%s ExpatError %s", exc.path, exc.lineno, exc)
        return 1

    try:
        return SUBCOMMAND_TABLE[args.command](args)
    except EntityLoadError as exc:
        return report_error(exc)


def _start():
    developer_mode = 'PRJ_DEVELOP' in os.environ
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        # Ignore Ctrl-C
        pass
    except Exception as exc:  # pylint: disable=broad-except
        # Any uncaught errors are something we should send to an error dump,
        # if we are in developer mode though, we should enter the debugger.
        if developer_mode:
            pdb.post_mortem()
        else:
            logger.error("An unhandled exception occurred: %s", exc)
            try:
                with open("prj.errors", "w") as file_object:
                    traceback.print_exception(*sys.exc_info(), file=file_object)
                logger.error("Please include the 'prj.errors' file when submitting a bug report")
            except:  # pylint: disable=bare-except
                pass
            sys.exit(1)

if __name__ == "__main__":
    _start()
