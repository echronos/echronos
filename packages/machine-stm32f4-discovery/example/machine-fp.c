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

void
machine_fp_init(void)
{
    /* This is the example code given in the Cortex-M4 Devices Generic User Guide */
    __asm volatile(
        "ldr.w r0, =0xe000ed88\n"
        "ldr r1, [r0]\n"
        "orr r1, r1, #(0xf << 20)\n"
        "str r1, [r0]\n"
        "dsb\n"
        "isb\n");
}

void
machine_fp_test_zero_nonfp_regs(void)
{
    /* Some of these are nonvolatile, and zeroing them is rather pointless because the compiler is supposed to
     * preserve them due to their being in the clobber list, but do it anyway to make sure the compiler's also sane. */
    asm volatile(
            "mov r0, #0\n"
            "mov r1, #0\n"
            "mov r2, #0\n"
            "mov r3, #0\n"
            "mov r4, #0\n"
            "mov r5, #0\n"
            "mov r6, #0\n"
            "mov r7, #0\n"
            "mov r8, #0\n"
            "mov r9, #0\n"
            "mov r10, #0\n"
            "mov r11, #0\n"
            "mov r12, #0"
            ::: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12");
}
