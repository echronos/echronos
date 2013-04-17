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


class StandardDarwin(Standard):
    platforms = ['x86_64-apple-darwin']
    name = 'std_darwin'
    enabled = True


class StandardLinux(Standard):
    platforms = ['x86_64-unknown-linux-gnu']
    name = 'std_linux'
    enabled = True


class ProjectLinux(Standard):
    platforms = ['x86_64-unknown-linux-gnu']
    name = 'smaccm'
    enabled = True
    license = """
Copyright (c) 2013 National ICT Australia (NICTA), ABN 62 102 206 173. All
rights reserved except as specified herein.

Licensed by NICTA to Galois, Inc., Rockwell Collins, and the University of
Minnesota under the terms of the respective SMACCM Project Licence Agreement.

SPECIAL LICENSE RIGHTS
Agreement No.: FA8750-12-9-0179
Recipient's Name: NICTA
Recipient's Address: Level 5, 13 Garden Street, Eveleigh,
                     New South Wales, Australia

The Government’s rights to use, modify, reproduce, release, perform and
display, or disclose this technical data or computer software are restricted
by Agreement No. FA8750-12-9-0179, License No. 1. Any reproduction of
technical data, computer software, or portions thereof marked with this legend
must also reproduce the markings.
"""
