/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @TAG(NICTA_AGPL)
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
