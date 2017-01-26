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

#include <stdint.h>
#include <debug.h>

#define PPCE500_MSR_SPE_BIT 0x2000000

void
machine_fp_init(void)
{
{{#all_tasks_spe_enable}}
    uint32_t current_msr;

    asm volatile("mfmsr %0":"=r"(current_msr)::);
    /* Enable MSR[SPE] */
    current_msr |= PPCE500_MSR_SPE_BIT;
    asm volatile("mtmsr %0"::"r"(current_msr):);
{{/all_tasks_spe_enable}}
}

void
machine_fp_test_zero_nonfp_regs(void)
{
{{^all_tasks_spe_enable}}
    uint32_t current_msr;

    /* Assert that MSR[SPE] is not enabled in this task */
    asm volatile("mfmsr %0":"=r"(current_msr)::);
    if (current_msr & PPCE500_MSR_SPE_BIT) {
        debug_println("MSR[SPE] is enabled! Halting.\n");
        while (1);
    }
{{/all_tasks_spe_enable}}

    /* Zeroing the nonvolatile registers (r14-r30) here is rather pointless because the compiler is supposed to
     * preserve them due to their being in the clobber list, but do it anyway to make sure the compiler is also sane.
     * Note that we can't touch r31 because it's needed for correct functioning of inline asm. */
    asm volatile("li 3,0\n"
            "li 4,0\n"
            "li 5,0\n"
            "li 6,0\n"
            "li 7,0\n"
            "li 8,0\n"
            "li 9,0\n"
            "li 10,0\n"
            "li 11,0\n"
            "li 12,0\n"
            "li 14,0\n"
            "li 15,0\n"
            "li 16,0\n"
            "li 17,0\n"
            "li 18,0\n"
            "li 19,0\n"
            "li 20,0\n"
            "li 21,0\n"
            "li 22,0\n"
            "li 23,0\n"
            "li 24,0\n"
            "li 25,0\n"
            "li 26,0\n"
            "li 27,0\n"
            "li 28,0\n"
            "li 29,0\n"
            "li 30,0"
            :::"r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14", "r15", "r16", "r17", "r18",
        "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30");
}
