from pylib.components import Component
from pylib.release import Release


CORE_CONFIGURATIONS = {"posix": ["sched-rr-test", "sched-prio-inherit-test", "simple-mutex-test",
                                 "blocking-mutex-test", "simple-semaphore-test", "sched-prio-test",
                                 "acamar", "gatria", "kraz"],
                       "armv7m": ["acamar", "gatria", "kraz", "acrux", "rigel"],
                       "ppce500": ["acamar", "gatria", "kraz", "acrux", "kochab"]}

CORE_SKELETONS = {
    'sched-rr-test': [Component('reentrant'),
                      Component('sched-rr', {'assume_runnable': False}),
                      Component('sched-rr-test'),
                      ],
    'sched-prio-test': [Component('reentrant'),
                        Component('sched-prio', {'assume_runnable': False}),
                        Component('sched-prio-test'),
                        ],
    'sched-prio-inherit-test': [Component('reentrant'),
                                Component('sched-prio-inherit', {'assume_runnable': False}),
                                Component('sched-prio-inherit-test'),
                                ],
    'simple-mutex-test': [Component('reentrant'),
                          Component('simple-mutex'),
                          Component('simple-mutex-test'),
                          ],
    'blocking-mutex-test': [Component('reentrant'),
                            Component('blocking-mutex'),
                            Component('blocking-mutex-test'),
                            ],
    'simple-semaphore-test': [Component('reentrant'),
                              Component('simple-semaphore'),
                              Component('simple-semaphore-test'),
                              ],
    'acamar': [Component('reentrant'),
               Component('acamar'),
               Component('stack', pkg_component=True),
               Component('context-switch', pkg_component=True),
               Component('error'),
               Component('task'),
               ],
    'gatria': [Component('reentrant'),
               Component('stack', pkg_component=True),
               Component('context-switch', pkg_component=True),
               Component('sched-rr', {'assume_runnable': True}),
               Component('simple-mutex'),
               Component('error'),
               Component('task'),
               Component('gatria'),
               ],
    'kraz': [Component('reentrant'),
             Component('stack', pkg_component=True),
             Component('context-switch', pkg_component=True),
             Component('sched-rr', {'assume_runnable': True}),
             Component('signal'),
             Component('simple-mutex'),
             Component('error'),
             Component('task'),
             Component('kraz'),
             ],
    'acrux': [Component('reentrant'),
              Component('stack', pkg_component=True),
              Component('context-switch', pkg_component=True),
              Component('sched-rr', {'assume_runnable': False}),
              Component('interrupt-event', pkg_component=True),
              Component('interrupt-event', {'timer_process': False, 'task_set': False}),
              Component('simple-mutex'),
              Component('error'),
              Component('task'),
              Component('acrux'),
              ],
    'rigel': [Component('reentrant'),
              Component('stack', pkg_component=True),
              Component('context-switch', pkg_component=True),
              Component('sched-rr', {'assume_runnable': False}),
              Component('signal'),
              Component('timer', pkg_component=True),
              Component('timer'),
              Component('interrupt-event', pkg_component=True),
              Component('interrupt-event', {'timer_process': True, 'task_set': True}),
              Component('blocking-mutex'),
              Component('profiling'),
              Component('message-queue'),
              Component('error'),
              Component('task'),
              Component('rigel'),
              ],
    'kochab': [Component('reentrant'),
               Component('stack', pkg_component=True),
               Component('context-switch', pkg_component=True),
               Component('sched-prio-inherit', {'assume_runnable': False}),
               Component('signal'),
               Component('blocking-mutex'),
               Component('simple-semaphore'),
               Component('error'),
               Component('task'),
               Component('kochab'),
               ]
}



class Standard(Release):
    packages = ['armv7m', 'generic', 'rtos-example', 'machine-qemu-simple', 'machine-stm32f4-discovery']
    platforms = ['x86_64-apple-darwin', 'x86_64-unknown-linux-gnu']
    version = '0.0.2'
    product_name = 'eChronos'
    release_name = 'std'
    enabled = True
    license = """
Copyright (c) NIPR Pty Ltd 2013.
UNLICENSED: Not for use outside NICTA or Breakaway Consulting.
"""
    top_level_license = """Unpublished copyright (c) 2013 National ICT Australia (NICTA).
ABN 62 102 206 173.  All rights reserved.
"""


class StandardDarwin(Standard):
    platforms = ['x86_64-apple-darwin']
    release_name = 'std_darwin'
    enabled = True


class StandardLinux(Standard):
    platforms = ['x86_64-unknown-linux-gnu']
    release_name = 'std_linux'
    enabled = True


class SmaccmBase(Standard):
    license = """
Unpublished copyright (c) 2013 National ICT Australia (NICTA),
ABN 62 102 206 173.  All rights reserved.

The contents of this document are proprietary to NICTA and you may not
use, copy, modify, sublicense or distribute the contents in any form
except as permitted under the terms of a separately executed licence
agreement with NICTA.

COMMERCIAL LICENSE RIGHTS
Agreement No.: FA8750-12-9-0179
Contractor's Name; Rockwell Collins, Inc.
Contractor's Address: 400 Collins Road N.E., Cedar Rapids, IA 52498

By accepting delivery of the RTOS Code and Documentation, the Licensee
agrees that the software is "commercial" computer software within the
meaning of the applicable acquisition regulations (e.g., FAR 2.101 or
DFARS 227.7202-3).  The terms and conditions of this License shall pertain
to the Licensee's use and disclosure of the software, and shall supersede
any conflicting contractual terms or conditions.

"""
    top_level_license = """The contents of this package are proprietary to NICTA and you may not
use, copy, modify, sublicense or distribute the contents in any form
except as permitted under the terms of a separately executed license
agreement with NICTA, such as (if applicable to you) one of the
following:

1. SMACCM Project License Agreement (Technical Area 4), by and between
NICTA and Rockwell Collins.

2. SMACCM Project Licence Agreement (Technical Area 4), by and between
NICTA and Regents of the Univeristy of Minnesota.

3. SMACCM Project Licence Agreement (Technical Area 3), by and between
NICTA and Galois, Inc.

4. SMACCM Project Licence Agreement (Technical Area 5), by and between
NICTA and Assured Information Security, Inc.


COMMERCIAL LICENSE RIGHTS
Agreement No.: FA8750-12-9-0179
Contractor's Name; Rockwell Collins, Inc.
Contractor's Address: 400 Collins Road N.E., Cedar Rapids, IA 52498

By accepting delivery of the RTOS Code and Documentation, the Licensee
agrees that the software is "commercial" computer software within the
meaning of the applicable acquisition regulations (e.g., FAR 2.101 or
DFARS 227.7202-3).  The terms and conditions of this License shall pertain
to the Licensee's use and disclosure of the software, and shall supersede
any conflicting contractual terms or conditions.
"""
    extra_files = [
        ('README.md', 'docs/smaccm_readme'),
        ('rigel-manual.pdf', 'docs/rigel-manual.pdf')
    ]


class SmaccmLinux(SmaccmBase):
    platforms = ['x86_64-unknown-linux-gnu']
    release_name = 'smaccm'
    enabled = True


class SmaccmDarwin(SmaccmBase):
    platforms = ['x86_64-apple-darwin']
    release_name = 'smaccm-darwin'
    enabled = True


class SmaccmPowerPCe500(SmaccmBase):
    packages = ['ppce500', 'generic', 'rtos-example', 'machine-qemu-ppce500']
    platforms = ['x86_64-unknown-linux-gnu']
    version = 'Aug2014'
    release_name = 'smaccm-ppce500'
    enabled = True
    extra_files = [
        ('README.md', 'docs/smaccm_ppc_readme'),
    ]
    license = """
Unpublished copyright (c) 2014 National ICT Australia (NICTA),
ABN 62 102 206 173.  All rights reserved.

The contents of this document are proprietary to NICTA and you may not
use, copy, modify, sublicense or distribute the contents in any form
except as permitted under the terms of a separately executed licence
agreement with NICTA.

COMMERCIAL LICENSE RIGHTS
Agreement No.: FA8750-12-9-0179
Contractor's Name; Rockwell Collins, Inc.
Contractor's Address: 400 Collins Road N.E., Cedar Rapids, IA 52498

By accepting delivery of the eChronos Code and Documentation, the Licensee
agrees that the software is "commercial" computer software within the
meaning of the applicable acquisition regulations (e.g., FAR 2.101 or
DFARS 227.7202-3).  The terms and conditions of this License shall pertain
to the Licensee's use and disclosure of the software, and shall supersede
any conflicting contractual terms or conditions.
    """
    top_level_license = """
The contents of this package are proprietary to National ICT Australia Limited
(NICTA), ABN 62 102 206 173 and you may not use, copy, modify, sublicense or
distribute the contents in any form except as permitted under the terms of a
separately executed license agreement with NICTA, such as (if applicable to
you) one of the following:

1. SMACCM Project License Agreement (Technical Area 4), by and between
NICTA and Rockwell Collins, effective 29 January 2013.

2. SMACCM Project Licence Agreement (Technical Area 4), by and between
NICTA and Regents of the University of Minnesota, effective 5 April 2013.

3. SMACCM Project Licence Agreement (Technical Area 3), by and between
NICTA and Galois, Inc., effective 21 February 2013.

4. SMACCM Project Licence Agreement (Technical Area 1), by and between
NICTA and The Boeing Company, effective 27 June 2013.


COMMERCIAL LICENSE RIGHTS
Agreement No.: FA8750-12-9-0179
Contractor's Name; Rockwell Collins, Inc.
Contractor's Address: 400 Collins Road N.E., Cedar Rapids, IA 52498

The sources include the eChronos RTOS for the Power PC e500 platform,
including tools, example applications, and supporting documentation.

By accepting delivery of the eChronos Code and Documentation, the Licensee
agrees that the software is "commercial" computer software within the
meaning of the applicable acquisition regulations (e.g., FAR 2.101 or
DFARS 227.7202-3).  The terms and conditions of this License shall pertain
to the Licensee's use and disclosure of the software, and shall supersede
any conflicting contractual terms or conditions.
    """
