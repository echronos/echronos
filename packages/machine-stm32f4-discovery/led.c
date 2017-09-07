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
