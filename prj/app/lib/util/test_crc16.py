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

import unittest
from util.crc16 import Crc16Ccitt, crc16ccitt


class TestCase(unittest.TestCase):
    def test_empty(self):
        self.assertEqual(crc16ccitt(), 0xffff)

    def test_expected(self):
        # Test values determined from:
        #   http://www.lammertbies.nl/comm/info/crc-calculation.html
        for inp, expected in [
                (b'0', 0xD7A3),
                (b'0a', 0x641D),
                (b'123456789', 0x29B1),
                (b'foo_bar', 0x37DF)]:
            self.assertEqual(crc16ccitt(inp), expected)

    def test_multi(self):
        for inp, expected in [
                ((b'0', b'a'), 0x641D),
                ((b'1234', b'56789'), 0x29B1),
                ((b'foo_', b'bar'), 0x37DF)]:
            self.assertEqual(crc16ccitt(*inp), expected)

    def test_reuse(self):
        crc = Crc16Ccitt()

        for byte in b'foo':
            crc.add(byte)
        self.assertEqual(crc.result(reset=False), 0x630A)

        for byte in b'bar':
            crc.add(byte)
        self.assertEqual(crc.result(reset=False), 0xBE35)

        crc.reset()

        for byte in b'foo':
            crc.add(byte)
        self.assertEqual(crc.result(), 0x630A)

        for byte in b'bar':
            crc.add(byte)
        self.assertEqual(crc.result(), 0x5F59)
