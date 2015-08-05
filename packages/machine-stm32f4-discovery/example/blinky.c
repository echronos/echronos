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
  <schema>
   <entry name="delay_amount" type="int" default="10000"/>
  </schema>
</module>*/
#include "led.h"
#define DELAY_AMOUNT {{delay_amount}}

static inline void
delay(void)
{
    volatile int i;
    for (i = 0; i < DELAY_AMOUNT; i++)
    {
    }
}

int
main(void)
{
    unsigned int i;
    led_init();

    for (i = 0; ; i++)
    {
        switch (i % 8)
        {
        case 0:
            led_green_on();
            break;
        case 1:
            led_blue_on();
            break;
        case 2:
            led_red_on();
            break;
        case 3:
            led_orange_on();
            break;
        case 4:
            led_green_off();
            break;
        case 5:
            led_blue_off();
            break;
        case 6:
            led_red_off();
            break;
        case 7:
            led_orange_off();
            break;
        }
        delay();
    }
}
