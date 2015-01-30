/* @LICENSE(NICTA) */

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
