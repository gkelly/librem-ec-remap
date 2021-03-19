// SPDX-License-Identifier: GPL-3.0-only
//
// TI BQ24715 Smart Charger
// https://www.ti.com/lit/ds/symlink/bq24780s.pdf

#include <board/battery.h>
#include <board/smbus.h>
#include <common/debug.h>

// ChargeOption0 flags
// ChargeInhibit
// 1=inihibt charge
#define SBC_CHARGE_INHIBIT  ((uint16_t)(1 << 0))
// 1=enable
#define SBC_IDPM_EN         ((uint16_t)(1 << 1))
// 1=enable, precharge clamped below 384mA
#define SBC_LDO_MODE_EN     ((uint16_t)(1 << 2))
// 0: switch IOUT based on IDPM_EN <default @ POR>
// 1: select IOUT based on bit[4]
#define SBC_FIX_IOUT        ((uint16_t)(1 << 3))
// When bit[3]=1, bit[4] serve as input
// 0: IOUT is the 40x adapter current amplifier output
// 1: IOUT is the 16x discharge current amplifier output
// When bit[3]=0, bit[4] serve as output (indicate IOUT selection)
// 0: IOUT the 40x adapter current amplifier output; when IDPM is disabled <default @ POR>
// 1: IOUT is the 16x discharge current amplifier output; when IDPM is enabled
#define SBC_IOUT_SEL        ((uint16_t)(1 << 4))
// 0: Disable LEARN Mode <default @ POR>
// 1: Enable LEARN Mode
#define SBC_LEARN_EN        ((uint16_t)(1 << 5))
// 0: 250mV
// 1: 350mV <default>
#define SBC_LSFET_OCP_THR   ((uint16_t)(1 << 6))
// 0: function is disable <default @ POR>
// 1: 333% IDPM
#define SBC_ACOC            ((uint16_t)(1 << 7))
// Switching Frequency
// 00:600kHz
// 01:800kHz <default @ POR>
// 10: 1MHz
//11: 800 kHz
#define SBC_PWM_FREQ_800KHZ ((uint16_t)(0b01 << 8))
// AudioFrequencyLimit
// 0: No limit of switching frequency <default @ POR>
// 1: Set minimum switching frequency to 40 kHz to avoid audio noise
#define SBC_AUDIO_FREQ_LIM  ((uint16_t)(1 << 10))
// 0: not OVP (default; write 0 to clear the OVP status)
// 1: OVP latch (read only)
#define SBC_SYSOVP_STAT     ((uint16_t)(1 << 11))
//Converter latch-off when sysovp is detected.
// 0: 15.1 V for 3s; 10.1 V for 2s <default @ POR>
// 1: 17.0 V for 3s, 11.3 V for 2s
#define SBC_SYSOVP          ((uint16_t)(1 << 12))
// Watchdog Timer Adjust
// 00: Disable Watchdog Timer
// 01: Enabled, 44 sec
// 10: Enabled, 88 sec
// 11: Enable Watchdog Timer (175s) <default @ POR>
#define SBC_WDTMR_ADJ_175S  ((uint16_t)(0b11 << 13))
// Low Power Mode Enable
// Effective on BAT power only (ACDET<0.6 or VCC<UVLO)
// 0: turn on discharge current monitoring
// 1: lower quiescent current, discharge current monitoring are turned off <default @ POR>
#define SBC_EN_LWPWR        ((uint16_t)(1 << 15))


static bool charger_enabled = true; // charger is enable by default at POR

int battery_charger_disable(void) {
    int res = 0;

    if (!charger_enabled)
        return 0;

    DEBUG("CHG disable\n");

#if 0
    // Disable charge voltage
    res = smbus_write(CHARGER_ADDRESS, 0x15, 0);
    if (res < 0)
        return res;

    // Disable input current
    res = smbus_write(CHARGER_ADDRESS, 0x3F, 0);
    if (res < 0)
        return res;

    // Disable charge current
    res = smbus_write(CHARGER_ADDRESS, 0x14, 0);
    if (res < 0)
        return res;
#endif

    // Set charge option 0 with 175s watchdog
    res = smbus_write(
        CHARGER_ADDRESS,
        0x12,
        // SBC_CHARGE_INHIBIT | SBC_LDO_MODE_EN | SBC_LSFET_OCP_THR | SBC_PWM_FREQ_800KHZ | SBC_WDTMR_ADJ_175S
        SBC_CHARGE_INHIBIT | SBC_IDPM_EN | SBC_LDO_MODE_EN | SBC_LSFET_OCP_THR | SBC_PWM_FREQ_800KHZ | SBC_WDTMR_ADJ_175S
    );
    DEBUG("CHG disabled\n");
    charger_enabled = false;

    return 0;
}

int battery_charger_enable(void) {
    int res = 0;

    if (charger_enabled) return 0;

    DEBUG("CHG enable @ %dmV %dmA\n", battery_charge_voltage, battery_charge_current);

    res = battery_charger_disable();
    if (res < 0)
        return res;

    // Set minimum system voltage
    res = smbus_write(CHARGER_ADDRESS, 0x3E, charger_min_system_voltage);
    if (res < 0)
        return res;

    // Set input current in mA
    res = smbus_write(CHARGER_ADDRESS, 0x3F, charger_input_current);
    if (res < 0)
        return res;

    // Set charge voltage in mV
    res = smbus_write(CHARGER_ADDRESS, 0x15, battery_charge_voltage);
    if (res < 0)
        return res;

    // Set charge current in mA,
    // setting the current to a valid value will enable charging!
    res = smbus_write(CHARGER_ADDRESS, 0x14, battery_charge_current);
    if (res < 0)
        return res;

    // Set charge option 0 with watchdog disabled
    res = smbus_write(
        CHARGER_ADDRESS,
        0x12,
        SBC_PWM_FREQ_800KHZ |
        SBC_IDPM_EN |
        SBC_FIX_IOUT |
        SBC_AUDIO_FREQ_LIM
    );

    DEBUG("CHG enabled\n");
    charger_enabled = true;
    return 0;
}

void battery_debug(void) {
    uint16_t data = 0;
    int res = 0;

    #define commandx(N, A, V) { \
        DEBUG("  " #N ": "); \
        res = smbus_read(A, V, &data); \
        if (res < 0) { \
            DEBUG("ERROR %04X\n", -res); \
        } else { \
            DEBUG("%04X\n", data); \
        } \
    }

    #define command(N, A, V) { \
        DEBUG("  " #N ": "); \
        res = smbus_read(A, V, &data); \
        if (res < 0) { \
            DEBUG("ERROR %04X\n", -res); \
        } else { \
            DEBUG("%d\n", data); \
        } \
    }

    DEBUG("Charger (bq24715):\n");
    commandx(ChargeOption0, CHARGER_ADDRESS, 0x12);
    command(ChargeCurrent, CHARGER_ADDRESS, 0x14);
    command(ChargeVoltage, CHARGER_ADDRESS, 0x15);
    command(MinSysVoltage, CHARGER_ADDRESS, 0x3E);
    command(InputCurrent, CHARGER_ADDRESS, 0x3F);
    commandx(ManufacturerID, CHARGER_ADDRESS, 0xFE);
    commandx(DeviceID, CHARGER_ADDRESS, 0xFF);

    #undef command
}

void battery_charger_event(void) {
    //TODO: watchdog
    //DEBUG("CHG event\n");
    //battery_debug();
}

