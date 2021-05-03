// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/board.h>
#include <board/gpio.h>
#include <board/power.h>
#include <board/kbc.h>
#include <ec/pwm.h>
#include <ec/adc.h>
#include <ec/bram.h>
#include <common/debug.h>

extern uint8_t main_cycle;


// persistent settings stored in BRAM bank#1 @ 0x80..0xbf
#define BRAM_OFFSET 0x80
#define BRAM_CHARGE_START_THRES		(BRAM_OFFSET + 0)
#define BRAM_CHARGE_END_THRES		(BRAM_OFFSET + 1)

// see if BRAM has valid content, if not clear it and mark
bool bram_init(void) {
    int i;
    uint8_t sum=42;

    for (i=0x80; i<(0xbf); i++)
        sum ^= BRAM[i];
    if (sum == BRAM[0xbf]) {
        return true;
    } else {
        for (i=0x80; i<(0xbf); i++)
            BRAM[i] = 0x00;
        BRAM[0xbf] = 42;
        return false;
    }
}

bool bram_set_value(uint8_t offset, uint8_t value)
{
    int i;
    uint8_t sum=42;

    if (offset < 0x80)
        return false;

    BRAM[offset] = value;
    for (i=0x80; i<(0xbf); i++)
        sum ^= BRAM[i];
    BRAM[0xbf] = sum;

    return true;
}

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
    // Turn off fans, get controlled by PECI
    DCR0 = 0x00;
    DCR1 = 0x00;

    // turn off notification LED RGB
    DCR2 = 0x00; // B
    DCR3 = 0x00; // G
    DCR4 = 0x00; // R

    // enable power LED control full brightness
    DCR5 = 0xff;

    // keyboard backlight PWM, zero at init
    DCR6 = 0x00;

    board_battery_init();
    battery_reset();

    if (bram_init) {
        battery_set_start_threshold(BRAM[BRAM_CHARGE_START_THRES]);
        battery_set_end_threshold(BRAM[BRAM_CHARGE_END_THRES]);
    }
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
    if (main_cycle == 0) {
        // Set keyboard LEDs
        static uint8_t last_kbc_leds = 0;
        if (kbc_leds != last_kbc_leds) {
            gpio_set(&LED_CAPS_LOCK, (kbc_leds & 4));
            last_kbc_leds = kbc_leds;
        }
    }
}

// called once per second
void board_1s_event(void) {
    if (power_state == POWER_STATE_S0) {
        if (!gpio_get(&HEADPHONE_DET)) {
            // there is a pull up so setting it as input will pull it high too
            GPCRF0 = GPIO_IN;
        } else {
            // pin state is false by GPIO init, so just reenable output to pull it low
            GPCRF0 = GPIO_OUT;
        }
    } else {
        gpio_set(&MIC_SELECT, false);
	}
}
