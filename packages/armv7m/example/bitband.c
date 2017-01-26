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

#include <stdint.h>
#include "debug.h"
#include "bitband.h"

#define BAR bar
#define U8 uint8_t
BITBAND_VAR(U8, BAR); BITBAND_VAR(uint8_t, bar2); BITBAND_VAR(uint32_t, foo);

/*
 *  A larger array would overflow the bitband region and cause a build
 * error.
 */
BITBAND_VAR_ARRAY(uint32_t, foo2, 262142);

int
main(void)
{
    foo = 5;
    debug_print("foo = ");
    debug_printhex32(foo);
    debug_println("");

    debug_print("foo_bitband[0] = ");
    debug_printhex32(foo_bitband[0]);
    debug_println("");

    debug_print("foo_bitband[1] = ");
    debug_printhex32(foo_bitband[1]);
    debug_println("");

    debug_print("foo_bitband[2] = ");
    debug_printhex32(foo_bitband[2]);
    debug_println("");

    debug_print("foo_bitband[3] = ");
    debug_printhex32(foo_bitband[3]);
    debug_println("");


    foo2[0] = 5;

    debug_print("foo2_bitband[0] = ");
    debug_printhex32(foo2_bitband[0]);
    debug_println("");

    return 0;
}
