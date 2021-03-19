// SPDX-License-Identifier: GPL-3.0-only

#include <stdbool.h>
#include <arch/delay.h>
#include <ec/adc.h>

void adc_enable(bool enable) {
    if (enable)
        ADCCFG |= (1 << 0);	// ADCEN
    else
        ADCCFG &= ~(1 << 0);
}

void adc_init(void) {
//    adc_enable(true);
    ADCSTS |= (1 << 2);
    delay_us(1300);
    ADCSTS &= ~(1 << 2);

    ADCSTS |= (1 << 3);		// AINITB, only run once after power up
    delay_us(100);
    ADCSTS &= ~(1 << 3);

    ADCSTS |= (1 << 2);
    delay_us(1300);
    ADCSTS &= ~(1 << 2);

    ADCGCR |= (1L << 7);	// keep buffer enable ADCDBKEN
    KDCTL = 0x80;		// enable auto hardware calibration AHCE
//    adc_enable(false);
}

