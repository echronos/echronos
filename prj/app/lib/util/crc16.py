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

"""Implement the CCITTT-16 CRC algorithm.

Unfortunately the built in Python modules don't provide
implementations for CRC-16 so a custom implementation
is required.

"""
from util.util import s16l


class Crc16Ccitt(object):
    """This class implement CCITT-16 CRC engine.

    A simple example:

    c = Crc16Ccitt()
    c.add('x')
    c.add('y')
    c.add('z')
    c.result()

    """
    def __init__(self):
        self.state = None
        self.reset()

    def reset(self):
        """Reset the state of the CRC engine."""
        self.state = 0xffff

    def add(self, byte):
        """Add a new byte to the CRC engine. 'byte'
        should be a python character. E.g: c.add('x')

        """
        # CRC-16 polynomial
        poly_s = byte ^ (self.state >> 8)
        poly_t = poly_s ^ (poly_s >> 4)
        result = s16l(self.state, 8) ^ poly_t ^ s16l(poly_t, 5) ^ s16l(poly_t, 12)
        self.state = result

    def result(self, reset=True):
        """Return the CRC result.

        If reset is True the CRC engine will also be reset.

        """
        result = self.state
        if reset:
            self.reset()
        return result


def crc16ccitt(*datas):
    """Return the CRC16-CCITT of data.

    Multiple data parameters can be passed to this function;
    a CRC of all the data will be returned. This avoids the
    need for a caller to perform expensive string concatenation.

    E.g:

    >>> hex(crc16ccitt(b'foo'))
    0x630a
    >>> hex(crc16ccitt(b'foo', b'bar', b'baz'))
    0x0d53

    """
    crc = Crc16Ccitt()
    for data in datas:
        for char in data:
            crc.add(char)
    return crc.result()
