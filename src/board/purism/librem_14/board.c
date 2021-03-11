// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/board.h>
#include <board/gpio.h>
#include <board/power.h>
#include <ec/pwm.h>
#include <ec/adc.h>
#include <common/debug.h>

extern uint8_t main_cycle;

void board_init(void) {
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);

    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);

    // Assert SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SWI_N, true);

    // in case we got powered up running from battery only
    // we need to make sure power keeps up
    // gpio_set(&SMC_SHUTDOWN_N, true);

    adc_init();
    VCH0CTL = (1 << 0);	// VCH0 = ADC input 1 on GPI1, clear all else, bat voltage
    VCH1CTL = 0x00;	// VCH1 = ADC input 0 on GPI0, clear all else, charge/discharge currrent
    adc_enable(true); // we need ADC channel 1 to read bat voltage

    // I2C3 enable
    GCR2 |= (1 << 5); // SMB3E

    // PWMs
    // Turn on CPU fan a bit (further temperature control in peci_event)
    DCR0 = 0x50;
    DCR1 = 0x50;

    // turn off notification LED RGB
    DCR2 = 0x00; // B
    DCR3 = 0x00; // G
    DCR4 = 0x00; // R

    // enable power LED control full brightness
    DCR5 = 0xff;

    // enable keyboard backlight
    DCR6 = 0xff;

    board_battery_init();
    battery_reset();
}

void board_on_ac(bool ac) {
    DEBUG("board ac %s\n", ac ? "t" : "f");
    // as long as we are on AC we keep the EC powered
#if 0
    if (ac)
        gpio_set(&SMC_SHUTDOWN_N, false);
    else {
        // if we are _running_ from battery power, keep running
        if (power_state == POWER_STATE_DS3 ||
            power_state == POWER_STATE_S3 ||
            power_state == POWER_STATE_S0) {
                gpio_set(&SMC_SHUTDOWN_N, true);
        } else
                gpio_set(&SMC_SHUTDOWN_N, false);
    }
#endif
}

// called every main loop cycle, careful
void board_event(void) {
static uint16_t cled=0;

#if 0
    DCR2 = (cled & 0x0300) == 0 ? (cled & 0xff) : 0x00;
    DCR3 = (cled & 0x0300) == 0x0100 ? (cled & 0xff) : 0x00;
    DCR4 = (cled & 0x0300) == 0x0200 ? (cled & 0xff) : 0x00;
    cled++;
    if (cled > (3 * 0xff))
        cled=0;
#endif
}

// called once per second
void board_1s_event(void) {}
