from pylib.release import Release


class Standard(Release):
    packages = ['armv7m', 'generic', 'rtos-example', 'machine-qemu-simple', 'machine-stm32f4-discovery',
                'machine-armv7m-common']
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
