// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <board/battery.h>
#include <board/smbus.h>
#include <board/gpio.h>
#include <board/board.h>
#include <board/power.h>
#include <common/debug.h>
#include <ec/adc.h>

// Default values to disable battery charging thresholds
#define BATTERY_START_DEFAULT   90
#define BATTERY_END_DEFAULT     100

// Represents a battery percentage level, below which charging will begin.
// Valid values are [0, 100]
// A value of 0 turns off the start threshold control.
static uint8_t battery_start_threshold = BATTERY_START_DEFAULT;

// Represents a battery percentage level, above which charging will stop.
// Valid values are [0, 100]
// A value of 100 turns off the stop threshold control.
static uint8_t battery_end_threshold = BATTERY_END_DEFAULT;

uint8_t battery_get_start_threshold(void) {
    if (battery_start_threshold > 100)
        return BATTERY_START_DEFAULT;
    return battery_start_threshold;
}

bool battery_set_start_threshold(uint8_t value) {
    if (value > 100 || value >= battery_end_threshold)
        return false;

    battery_start_threshold = value;
    bram_set_value((uint8_t)BRAM_CHARGE_START_THRES, battery_start_threshold);
    return true;
}

uint8_t battery_get_end_threshold(void) {
    if (battery_end_threshold > 100)
        return BATTERY_END_DEFAULT;
    return battery_end_threshold;
}

bool battery_set_end_threshold(uint8_t value) {
    if (value > 100 || value <= battery_start_threshold)
        return false;

    battery_end_threshold = value;
    bram_set_value((uint8_t)BRAM_CHARGE_END_THRES, battery_end_threshold);
    return true;
}

/**
 * Configure the charger based on charging threshold values.
 */
int battery_charger_configure(void) {
    static bool should_charge = false;
    bool charger_present = !gpio_get(&ACIN_N);

    if (battery_charge >= battery_get_end_threshold()) {
        // Stop threshold configured: Stop charging at threshold.
        should_charge = false;
    }
    else if (battery_charge <= battery_get_start_threshold()) {
        // Start threshold configured: Start charging at threshold.
        should_charge = true;
    }

    if (battery_present && should_charge && charger_present) {
        gpio_set(&LED_BAT_CHG, false);
        // while 'on' only light the charging LED
        if (power_state == POWER_STATE_DS3 ||
            power_state == POWER_STATE_S3 ||
            power_state == POWER_STATE_S0) {
            gpio_set(&LED_PWR, true);
            gpio_set(&LED_BAT_WARN, true);
        }
        return battery_charger_enable();
    } else {
        // charging has been stopped or interrupted -> clear flag
        should_charge = false;
        // set appropriate LED state
        gpio_set(&LED_BAT_CHG, true);
        if (power_state == POWER_STATE_DS3 ||
            power_state == POWER_STATE_S3 ||
            power_state == POWER_STATE_S0) {
            if (battery_charge < 20) {
                gpio_set(&LED_BAT_WARN, false);
                gpio_set(&LED_PWR, true);
            } else {
                gpio_set(&LED_PWR, false);
                gpio_set(&LED_BAT_WARN, true);
            }
        }
        return battery_charger_disable();
    }
}

uint16_t battery_temp = 0;
uint16_t battery_voltage = 0;
uint16_t battery_current = 0;
uint16_t battery_charge = 100; // assume 100% for a start
uint16_t battery_remaining_capacity = 0;
uint16_t battery_full_capacity = 0;
uint16_t battery_status = 0;
uint16_t battery_design_capacity = 0;
uint16_t battery_design_voltage = 0;
uint16_t battery_cycle_count = 0;
uint16_t battery_manufacturing_date = 0;
uint16_t battery_charge_voltage = 0;
uint16_t battery_charge_current = 0;
uint16_t battery_min_voltage = 0;

uint16_t charger_input_current = 0x00;
uint16_t charger_min_system_voltage = 0x1600;

// default to false, will be updated when GPIO state checked in board_battery.c
bool battery_present = false;

void battery_event(void) {
    if (battery_present) {
        board_battery_update_state(); // this will update all available runtime values
        // DEBUG("BAT %d mV %d mA %d %%\n", battery_voltage, battery_current, battery_charge);
    }

    battery_charger_configure();
    battery_charger_event();
}

void battery_reset(void) {
    DEBUG("battery_reset()\n");
    battery_set_start_threshold(BATTERY_START_DEFAULT);
    battery_set_end_threshold(BATTERY_END_DEFAULT);
}
