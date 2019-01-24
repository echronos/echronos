#
# eChronos Real-Time Operating System
# Copyright (c) 2018, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#
import os.path
from pylib import tests


class Acamar(tests.AvrTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'acamar.prx')


class Acrux(tests.AvrTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'acrux.prx')


class Gatria(tests.AvrTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gatria.prx')


class Kraz(tests.AvrTestCase):
    prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'kraz.prx')
