/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2017, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */
#if __MINGW32__ == 1
/* MinGW GCC does not place STDOUT_FILENO or _exit in the header file
defined by the POSIX standard.  It is not clear why this is, however
for now a work-around is provided. */
#include <stdio.h>
#include <stdlib.h>
#endif

#include <unistd.h>
#include <string.h>

void
debug_puts(const char *s)
{
    ssize_t l = strlen(s);
    if (write(STDOUT_FILENO, s, l) != l)
    {
        _exit(1);
    }
}
