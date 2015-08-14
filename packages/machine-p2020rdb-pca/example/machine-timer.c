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

void
machine_timer_clear(void)
{
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[FIS] (fixed-interval timer interrupt status) */
        "lis %%r3,0x400\n"
        "mttsr %%r3\n"
        ::: "r3");
}

static void
decrementer_clear(void)
{
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[DIS] (decrementer interrupt status) */
        "lis %%r3,0x800\n"
        "mttsr %%r3\n"
        ::: "r3");
}

/* Useful primarily in case of bootloaders that make use of timer interrupts */
void
machine_timer_deinit(void)
{
    asm volatile(
        "li %%r3,0\n"
        "mttcr %%r3"
        ::: "r3");

    /* In case there are any decrementer interrupts pending, clear them */
    decrementer_clear();
}

void
machine_timer_init(void)
{
    /* U-Boot has the decrementer on, so we turn this off here */
    machine_timer_deinit();

    /*
     * Configure a fixed interval timer
     *
     * Enable:
     *  In TCR (timer control register)
     *    TCR[FIE] (fixed-interval interrupt enable)
     *
     * Set period: TCR[FPEXT] || TCR[FP]
     */
    asm volatile(
        /*
         * 1 TB (time base) period occurs every 8 CCB periods (by default setting of HID0[SEL_TBCLK]=0).
         *
         * On P2020, CCB (Core Complex Bus) clock frequency ranges from 800MHz to 1.3GHz.
         * 0x2810000 => 1000_10: TBL[33]: 30th bit from lsb: 2^30 = 1,073,741,824 TB periods
         *      At 800MHz this is ~11s, and at 1.3GHz this is ~7s.
         * 0x3810000 => 1000_11: TBL[34]: 29th bit from lsb: 2^29 ~ 3-5s
         * 0x3812000 => 1001_11: TBL[38]: about 1/3 of a second
         */
        "mftcr %%r3\n"
        "oris %%r3,%%r3,0x381\n"
        "ori %%r3,%%r3,0x2000\n"
        "mttcr %%r3"
        ::: "r3");
}
