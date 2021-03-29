// SPDX-License-Identifier: GPL-3.0-only

#include <board/pwm.h>

void pwm_init(void) {
    // Set T0CHSEL to TACH0A and T1CHSEL to TACH1A
    TSWCTLR = 0;

    // Disable PWM
    ZTIER = 0;

    // Set prescalar clock frequency to EC clock (9.2MHz)
    PCFSR = 0b01;

    // Use C0CPRS and CTR0 for all channels
    PCSSGL = 0;
    PCSSGH = 0x20;

    // Set clock prescaler to 0 + 1
    C0CPRS = 0;

    C6CPRS = 0x01;
    C6MCPRS = 0x00;

    // Set cycle time to 255 + 1
    CTR0 = 255;

    // disable open drain for all channels
    PWMODENR = 0x00;

    // Enable PWM
    ZTIER = (1 << 1);
}
