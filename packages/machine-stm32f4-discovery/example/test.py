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

import os.path
# 'from pylib.tests import Stm32f407QemuTestCase' would make the command
# 'x.py test units' attempt to run the unconfigured
# Stm32f407QemuTestCase class as a test case which fails.
# Therefore, import the tests module instead.
from pylib import tests


class Acamar(tests.Stm32f407QemuTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'acamar-system.prx')


class Kochab(tests.Stm32f407QemuTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'kochab-system.prx')
