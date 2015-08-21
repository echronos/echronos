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

from prj import SystemBuildError, Module, ModuleInstance, pystache_render, xml2dict, xml2schema, xml_parse_string
import logging
import os
import ply.cpp
import shutil
logger = logging.getLogger()


class EntryModule(Module):
    # SVCall and PendSV are specified as optional in the schema, but are implemented in the asm to default to 'reset'.
    # They are not available for config (attempts will raise an assembler error) if preemption support is enabled.
    xml_schema = """
<schema>
    <entry name="flash_load_addr" type="int" default="0" />
    <entry name="code_addr" type="int" default="0" />
    <entry name="data_addr" type="int" default="0x20000000" />
    <entry name="stack_size" type="int" default="0x1000" />
    <entry name="bitband_base" type="int" default="0x20000000" />
    <entry name="bitband_size" type="int" default="0x100000" />
    <entry name="bitband_alias" type="int" default="0x22000000" />

    <entry name="preemption" type="bool" optional="true" default="false" />

    <entry name="nmi" type="c_ident" default="reset" />
    <entry name="hardfault" type="c_ident" default="reset" />
    <entry name="memmanage" type="c_ident" default="reset" />
    <entry name="busfault" type="c_ident" default="reset" />
    <entry name="usagefault" type="c_ident" default="reset" />
    <entry name="svcall" type="c_ident" optional="true" />
    <entry name="debug_monitor" type="c_ident" default="reset" />
    <entry name="pendsv" type="c_ident" optional="true" />
    <entry name="systick" type="c_ident" default="reset" />
    <entry name="external_irqs" type="list" default="[]">
        <entry name="external_irq" type="dict">
          <entry name="number" type="int"/>
          <entry name="handler" type="c_ident" default="reset" />
        </entry>
    </entry>
</schema>"""

    files = [
        {'input': 'bitband.h'},
        {'input': 'vectable.s', 'render': True, 'type': 'asm'},
        {'input': 'default.ld', 'render': True, 'type': 'linker_script', 'stage': 'post_prepare'},
    ]

    def configure(self, xml_config):
        config = {}
        config['external_irqs'] = []
        config['bit_aliases'] = []  # A list of variables that should have bitband aliases created.

        config.update(super().configure(xml_config))
        # Fill in external IRQ vector list
        xirqs = [{'handler':'reset'}] * 240
        for xirq in config['external_irqs']:
            xirqs[xirq['number']] = xirq
        config['external_irqs'] = xirqs

        return config

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

        super().post_prepare(system, config)


module = EntryModule()
