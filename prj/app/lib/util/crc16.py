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
        s = byte ^ (self.state >> 8)
        t = s ^ (s >> 4)
        r = s16l(self.state, 8) ^ t ^ s16l(t, 5) ^ s16l(t, 12)
        self.state = r

    def result(self, reset=True):
        """Return the CRC result.

        If reset is True the CRC engine will also be reset.

        """
        r = self.state
        if reset:
            self.reset()
        return r


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
    c = Crc16Ccitt()
    for data in datas:
        for ch in data:
            c.add(ch)
    return c.result()
