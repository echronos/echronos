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
# 'from pylib.tests import Armv7mQemuTestCase' would make the command
# 'x.py test units' attempt to run the unconfigured
# Armv7mQemuTestCase class as a test case which fails.
# Therefore, import the tests module instead.
from pylib import tests


class Acamar(tests.Armv7mQemuTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'acamar-system.prx')


class Gatria(tests.Armv7mQemuTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gatria-system.prx')


class Kraz(tests.Armv7mQemuTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'kraz-system.prx')


class Kochab(tests.Armv7mQemuTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'kochab-system.prx')
