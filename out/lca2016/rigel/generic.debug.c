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

extern void debug_putc(char);

void
debug_print(const char *msg)
{
    while (*msg != '\x00')
    {
        debug_putc(*msg);
        msg++;
    }
}


void
debug_println(const char *const msg)
{
    debug_print(msg);
    debug_putc('\n');
}

static void
put_hexdigit(const uint8_t val)
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
    debug_putc(ch);
}

void
debug_printhex32(const uint32_t val)
{
    debug_putc('0');
    debug_putc('x');

    put_hexdigit((val >> 28) & 0xf);
    put_hexdigit((val >> 24) & 0xf);
    put_hexdigit((val >> 20) & 0xf);
    put_hexdigit((val >> 16) & 0xf);
    put_hexdigit((val >> 12) & 0xf);
    put_hexdigit((val >> 8) & 0xf);
    put_hexdigit((val >> 4) & 0xf);
    put_hexdigit((val >> 0) & 0xf);
}

void
debug_printhex8(const uint8_t val)
{
    debug_putc('0');
    debug_putc('x');

    put_hexdigit((val >> 4) & 0xf);
    put_hexdigit((val >> 0) & 0xf);
}
