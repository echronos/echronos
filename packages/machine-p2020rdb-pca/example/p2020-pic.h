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

#define PIC_SPURIOUS_VECTOR_CODE 0xffff
#define PIC_NUM_GLOBAL_TIMERS 8

/* Selected interfaces for config and operation of the P2020 PIC (Programmable Interrupt Controller).
 * Please see the "P2020 QorIQ Integrated Processor Reference Manual" (P2020RM) for more info on configurable internal
 * interrupt vectors (IIVs) and the process for servicing PIC interrupts by use of the IACK and EOI registers. */

void pic_iiv_duart_init(uint32_t priority, uint32_t vector);
uint32_t pic_iack_get(void);
void pic_eoi_put(void);
void pic_global_timer_init(unsigned int i, uint32_t priority, uint32_t vector, uint32_t base_count);
