from prj import SystemBuildError, Module, ModuleInstance, pystache_render, xml2dict, xml2schema, xml_parse_string
import logging
import os
import ply.cpp
import shutil
logger = logging.getLogger()

schema_xml = """<schema>
    <entry name="flash_load_addr" type="int" default="0" />
    <entry name="code_addr" type="int" default="0" />
    <entry name="data_addr" type="int" default="0x20000000" />
    <entry name="stack_size" type="int" default="0x1000" />
    <entry name="bitband_base" type="int" default="0x20000000" />
    <entry name="bitband_size" type="int" default="0x100000" />
    <entry name="bitband_alias" type="int" default="0x22000000" />

    <entry name="exception_reset" type="c_ident" default="reset" />
    <entry name="nmi" type="c_ident" default="reset" />
    <entry name="hardfault" type="c_ident" default="reset" />
    <entry name="memmanage" type="c_ident" default="reset" />
    <entry name="busfault" type="c_ident" default="reset" />
    <entry name="usagefault" type="c_ident" default="reset" />
    <entry name="svcall" type="c_ident" default="reset" />
    <entry name="debug_monitor" type="c_ident" default="reset" />
    <entry name="pendsv" type="c_ident" default="reset" />
    <entry name="systick" type="c_ident" default="reset" />

</schema>"""


class EntryModule(Module):
    def configure(self, xml_config):
        config = {}
        config['external_irqs'] = []
        config['bit_aliases'] = []  # A list of variables that should have bitband aliases created.

        config.update(xml2dict(xml_config, xml2schema(xml_parse_string(schema_xml))))

        return config

    def prepare(self, system, config):
        # Copy associated header
        header = os.path.join(__path__, 'bitband.h')
        path = os.path.join(system.output, 'gen', os.path.basename('bitband.h'))
        shutil.copy(header, path)

        # Apply template to vectable.s
        filename = os.path.join(__path__, 'vectable.s')
        path = os.path.join(system.output, 'gen', 'vectable.s')
        logger.info("Preparing: template %s -> %s", filename, path)
        pystache_render(filename, path, config)
        system.add_asm_file(path)

    def post_prepare(self, system, config):
        # Now find all the BITBAND variables in all the c_files.
        def cb(macro_name, expanded_args):
            bitband_macros = ('BITBAND_VAR', 'BITBAND_VAR_ARRAY',
                              'VOLATILE_BITBAND_VAR', 'VOLATILE_BITBAND_VAR_ARRAY')
            if macro_name in bitband_macros and \
                    len(expanded_args[1]) == 1 and \
                    expanded_args[1][0].type == 'CPP_ID':
                config['bit_aliases'].append(expanded_args[1][0].value)

        p = ply.cpp.Preprocessor(include_paths=system.include_paths,
                                 macro_callback=cb)

        for c_file in system.c_files:
            with open(c_file) as f:
                try:
                    p.parse(f.read(), c_file)
                except ply.cpp.CppError as e:
                    raise SystemBuildError(str(e))

        # Apply template to linker script.
        filename = os.path.join(__path__, 'default.ld')
        path = os.path.join(system.output, 'gen', 'default.ld')
        logger.info("Preparing: template %s -> %s", filename, path)
        pystache_render(filename, path, config)
        system.linker_script = path


module = EntryModule()
