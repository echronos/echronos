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

# gdb --batch <SYSTEM_BINARY> -x <THIS_FILE>
# Don't prompt for terminal input
set height 0
target remote localhost:1212
load
b debug_println
display rtos_internal_current_task
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
