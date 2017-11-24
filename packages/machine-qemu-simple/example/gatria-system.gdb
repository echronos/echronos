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

target remote :1234
# Don't prompt for terminal input
set height 0
b debug_println
b main
# A once-only starting sequence:
c
c
c
c
c
c
c
c
c
c
# First iteration of periodic sequence:
c
c
c
c
c
c
c
c
c
c
c
# The following is an arbitrarily chosen number of repetitions:
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
quit
y
