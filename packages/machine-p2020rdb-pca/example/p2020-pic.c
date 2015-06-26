/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that no right, title
 * or interest in or to any trade mark, service mark, logo or trade name
 * of NICTA or its licensors is granted.
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

#include <p2020-util.h>
#include "p2020-pic.h"
#include "debug.h"

/* PIC registers are 32 bits wide */
#define PIC_REGISTER_BASE (CCSRBAR + 0x40000)
#define PIC_GCR (volatile uint32_t *)(PIC_REGISTER_BASE + 0x1020)
#define PIC_CTPR_CPU0 (volatile uint32_t *)(PIC_REGISTER_BASE + 0x20080)
#define PIC_IACK_CPU0 (volatile uint32_t *)(PIC_REGISTER_BASE + 0x200a0)
#define PIC_EOI_CPU0 (volatile uint32_t *)(PIC_REGISTER_BASE + 0x200b0)

#define PIC_IIVPR_BASE (PIC_REGISTER_BASE + 0x10200)
/* x is the internal interrupt number, range 0 to 63 */
#define PIC_IIVPR(x) (volatile uint32_t *)(PIC_IIVPR_BASE + (32 * (x)))
#define PIC_IIVPR_PRIORITY_SHIFT 16

/* Global timer group A, x is valid from 0 to 3 */
#define PIC_GTBCRA(x) (volatile uint32_t *)(PIC_REGISTER_BASE + 0x1110 + (64 * (x)))
#define PIC_GTVPRA(x) (volatile uint32_t *)(PIC_REGISTER_BASE + 0x1120 + (64 * (x)))
/* Global timer group B, x is valid from 0 to 3 */
#define PIC_GTBCRB(x) (volatile uint32_t *)(PIC_REGISTER_BASE + 0x2110 + (64 * (x)))
#define PIC_GTVPRB(x) (volatile uint32_t *)(PIC_REGISTER_BASE + 0x2120 + (64 * (x)))
#define PIC_GTVPR_PRIORITY_SHIFT 16

#define PIC_EOI_CODE 0
#define PIC_IIV_DUART 26

static void
assert_priority_vector_valid(const char *caller, uint32_t priority, uint32_t vector)
{
    if (priority > 15) {
        debug_print(caller);
        debug_println(": Priority must be in range 0..15!");
        while (1);
    }

    if (vector > 0xffff) {
        debug_print(caller);
        debug_println(": Vector cannot be larger than 0xffff!");
        while (1);
    }
}

void
pic_iiv_duart_init(uint32_t priority, uint32_t vector)
{
    assert_priority_vector_valid(__func__, priority, vector);

    /* IIDR[P0]=1 (Interrupts directed to CPU0) is the default setting at reset, so we don't change that. */

    /* Set the DUART interrupt priority to a non-zero value so its delivery isn't inhibited */
    *PIC_IIVPR(PIC_IIV_DUART) |= (priority << PIC_IIVPR_PRIORITY_SHIFT);

    /* Set CTPR[TASKP] to some value lower than DUART's priority so that DUART irq is sent to CPU */
    *PIC_CTPR_CPU0 = 0;

    /* Set the DUART vector number to a recognisable, arbitrary value */
    *PIC_IIVPR(PIC_IIV_DUART) |= vector;

    /* Unmask the DUART interrupt */
    *PIC_IIVPR(PIC_IIV_DUART) &= 0x7fffffff;
}

uint32_t
pic_iack_get(void)
{
    /* PIC_IACK_CPU0 should return the vector of the highest priority pending interrupt.
     * Upon reading this register, the interrupt is considered to be in service until PIC_EOI_CPU0 is written. */
    return *PIC_IACK_CPU0;
}

void
pic_eoi_put(void)
{
    /* PIC_EOI_CPU0 signals end of processing for the highest priority interrupt currently in service. */
    *PIC_EOI_CPU0 = PIC_EOI_CODE;
}

void
pic_global_timer_init(int i, uint32_t priority, uint32_t vector, uint32_t base_count)
{
    if (i < 0 || i > 7) {
        /* No. Bad! */
        debug_print(__func__);
        debug_println(": There are only 8 timers - only i in range 0..7 are valid!");
        while (1);
    }

    if (base_count & 0x80000000) {
        debug_print(__func__);
        debug_println(": Base count cannot exceed 0x7fffffff!");
        while (1);
    }

    assert_priority_vector_valid(__func__, priority, vector);

    if (i < 4) {
        *PIC_GTVPRA(i) = (priority << PIC_GTVPR_PRIORITY_SHIFT) | vector;
        *PIC_GTBCRA(i) = base_count;
    } else {
        *PIC_GTVPRB(i) = (priority << PIC_GTVPR_PRIORITY_SHIFT) | vector;
        *PIC_GTBCRB(i) = base_count;
    }
}
