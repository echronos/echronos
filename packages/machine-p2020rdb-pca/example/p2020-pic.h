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

#define PIC_SPURIOUS_VECTOR_CODE 0xffff
#define PIC_NUM_GLOBAL_TIMERS 8

/* Selected interfaces for config and operation of the P2020 PIC (Programmable Interrupt Controller).
 * Please see the "P2020 QorIQ Integrated Processor Reference Manual" (P2020RM) for more info on configurable internal
 * interrupt vectors (IIVs) and the process for servicing PIC interrupts by use of the IACK and EOI registers. */

void pic_iiv_duart_init(uint32_t priority, uint32_t vector);
uint32_t pic_iack_get(void);
void pic_eoi_put(void);
void pic_global_timer_init(unsigned int i, uint32_t priority, uint32_t vector, uint32_t base_count);
