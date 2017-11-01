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

#ifndef LED_H
#define LED_H

void led_init(void);

void led_green_on(void);
void led_green_off(void);

void led_blue_on(void);
void led_blue_off(void);

void led_orange_on(void);
void led_orange_off(void);

void led_red_on(void);
void led_red_off(void);

#endif
