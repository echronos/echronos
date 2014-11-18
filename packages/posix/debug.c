
#if __MINGW32__ == 1
/* MinGW GCC does not place STDOUT_FILENO or _exit in the header file
defined by the POSIX standard.  It is not clear why this is, however
for now a work-around is provided. */
#include <stdio.h>
#include <stdlib.h>
#endif

#include <unistd.h>

void
rtos_internal_debug_putc(const char c)
{
    ssize_t r;
    r = write(STDOUT_FILENO, &c, sizeof c);
    if (r != 1)
    {
        _exit(1);
    }
}
