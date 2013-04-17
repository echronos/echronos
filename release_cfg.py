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
