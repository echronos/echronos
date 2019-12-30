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

/*<module>
  <code_gen>template</code_gen>
  <headers>
      <header path="debug.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="prefix" type="c_ident" default="" />
   <entry name="ll_debug" type="c_ident" default="" />
  </schema>
</module>*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <stdint.h>
#include "debug.h"

extern void {{ll_debug}}debug_puts(const char *);

void
{{prefix}}debug_print(const char *msg)
{
    {{ll_debug}}debug_puts(msg);
}


void
{{prefix}}debug_println(const char *const msg)
{
    {{prefix}}debug_print(msg);
    {{ll_debug}}debug_puts("\n");
}


void
debug_puts(const char *s)
{
    ssize_t l = strlen(s);
    if (write(STDOUT_FILENO, s, l) != l)
    {
        _exit(1);
    }
}
