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
#include "led.h"

#define RCC_BASE 0x40023800
#define RCC_REG(x) (RCC_BASE + (x))
#define RCC_AHB1ENR RCC_REG(0x30)

#define RCC_AHB1ENR_WRITE(x) (*((volatile uint32_t*)RCC_AHB1ENR) = x)

#define GPIO_D_BASE 0x40020C00
#define GPIO_D_REG(x) (GPIO_D_BASE + (x))

#define GPIO_D_BSRR GPIO_D_REG(0x18)
#define GPIO_D_MODER GPIO_D_REG(0x0)

#define DISABLE_OFFSET_BITS 16

#define GPIO_D_BSRR_WRITE(x) (*((volatile uint32_t*)GPIO_D_BSRR) = x)
#define GPIO_D_MODER_WRITE(x) (*((volatile uint32_t*)GPIO_D_MODER) = x)

#define GPIO_D_ON(x) GPIO_D_BSRR_WRITE((x))
#define GPIO_D_OFF(x) GPIO_D_BSRR_WRITE((x) << DISABLE_OFFSET_BITS)

#define GREEN_LED_MASK (0x1 << 12)
#define ORANGE_LED_MASK (0x1 << 13)
#define RED_LED_MASK (0x1 << 14)
#define BLUE_LED_MASK (0x1 << 15)

#define ALL_LED_MASK (GREEN_LED_MASK | ORANGE_LED_MASK | RED_LED_MASK | BLUE_LED_MASK)

#define GREEN_LED_ENABLE_MASK (0x1U << (12 * 2))
#define ORANGE_LED_ENABLE_MASK (0x1U << (13 * 2))
#define RED_LED_ENABLE_MASK (0x1U << (14 * 2))
#define BLUE_LED_ENABLE_MASK (0x1U << (15 * 2))

#define ALL_LED_ENABLE_MASK (GREEN_LED_ENABLE_MASK | ORANGE_LED_ENABLE_MASK | RED_LED_ENABLE_MASK | BLUE_LED_ENABLE_MASK)

void
led_green_on(void)
{
    GPIO_D_ON(GREEN_LED_MASK);
}

void
led_green_off(void)
{
    GPIO_D_OFF(GREEN_LED_MASK);
}

void
led_blue_on(void)
{
    GPIO_D_ON(BLUE_LED_MASK);
}

void
led_blue_off(void)
{
    GPIO_D_OFF(BLUE_LED_MASK);
}

void
led_orange_on(void)
{
    GPIO_D_ON(ORANGE_LED_MASK);
}

void
led_orange_off(void)
{
    GPIO_D_OFF(ORANGE_LED_MASK);
}

void
led_red_on(void)
{
    GPIO_D_ON(RED_LED_MASK);
}

void
led_red_off(void)
{
    GPIO_D_OFF(RED_LED_MASK);
}

void
led_init(void)
{
    RCC_AHB1ENR_WRITE(0x00100009); /* Enable GPIO clock */
    GPIO_D_MODER_WRITE(ALL_LED_ENABLE_MASK); /* Enable LED gpios as output */
    GPIO_D_BSRR_WRITE(ALL_LED_MASK << DISABLE_OFFSET_BITS); /* Turn off all LEDs */
}
