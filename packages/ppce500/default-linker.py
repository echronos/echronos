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

from prj import Module


class DefaultLinkerModule(Module):
    # Default stack size value is arbitrary.
    xml_schema = """
<schema>
    <entry name="load_addr" type="int" default="0" />
    <entry name="stack_size" type="int" default="0x1000" />
</schema>"""

    files = [
        {'input': 'default.ld', 'render': True, 'type': 'linker_script', 'stage': 'post_prepare'},
    ]


module = DefaultLinkerModule()  # pylint: disable=invalid-name
