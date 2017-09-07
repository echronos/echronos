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

static char
get_hexdigit(const uint8_t val)
{
    char ch;
    if (val < 10)
    {
        ch = '0' + val;
    }
    else if ((val >= 10) && (val < 16))
    {
        ch = 'a' + (val - 10);
    }
    else
    {
        ch = '?';
    }
    return ch;
}

void
{{prefix}}debug_printhex32(const uint32_t val)
{
    char str[11];
    str[0] = '0';
    str[1] = 'x';
    str[2] = get_hexdigit((val >> 28) & 0xf);
    str[3] = get_hexdigit((val >> 24) & 0xf);
    str[4] = get_hexdigit((val >> 20) & 0xf);
    str[5] = get_hexdigit((val >> 16) & 0xf);
    str[6] = get_hexdigit((val >> 12) & 0xf);
    str[7] = get_hexdigit((val >> 8) & 0xf);
    str[8] = get_hexdigit((val >> 4) & 0xf);
    str[9] = get_hexdigit((val >> 0) & 0xf);
    str[10] = 0;
    debug_print(str);
}

void
{{prefix}}debug_printhex8(const uint8_t val)
{
    char str[5];
    str[0] = '0';
    str[1] = 'x';
    str[2] = get_hexdigit((val >> 4) & 0xf);
    str[3] = get_hexdigit((val >> 0) & 0xf);
    str[4] = 0;
    debug_print(str);
}
