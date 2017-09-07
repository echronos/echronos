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


class P2020UtilModule(Module):
    # U-Boot on the P2020RDB-PCA inits the CCSRBAR to 0xffe00000
    xml_schema = """
<schema>
   <entry name="ccsrbar" type="int" default="0xffe00000" />
</schema>"""

    files = [
        {'input': 'p2020-util.h', 'render': True},
        {'input': 'p2020-util.c', 'render': True, 'type': 'c'},
    ]

module = P2020UtilModule()  # pylint: disable=invalid-name
