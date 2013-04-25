from x import Release


class Standard(Release):
    packages = ['armv7m', 'generic', 'rtos-example', 'machine-qemu-simple', 'machine-stm32f4-discovery']
    platforms = ['x86_64-apple-darwin', 'x86_64-unknown-linux-gnu']
    version = '0.0.1'
    name = 'std'
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
    name = 'std_darwin'
    enabled = True


class StandardLinux(Standard):
    platforms = ['x86_64-unknown-linux-gnu']
    name = 'std_linux'
    enabled = True


class ProjectBase(Standard):
    license = """
Unpublished copyright (c) 2013 National ICT Australia (NICTA),
ABN 62 102 206 173.  All rights reserved.

The contents of this document are proprietary to NICTA and you may not
use, copy, modify, sublicense or distribute the contents in any form
except as permitted under the terms of a separately executed licence
agreement with NICTA.

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
"""


class ProjectLinux(ProjectBase):
    platforms = ['x86_64-unknown-linux-gnu']
    name = 'smaccm'
    enabled = True


class ProjectDarwin(ProjectBase):
    platforms = ['x86_64-apple-darwin']
    name = 'smaccm-darwin'
    enabled = True
