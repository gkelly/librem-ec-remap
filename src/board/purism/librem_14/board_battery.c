// SPDX-License-Identifier: GPL-3.0-only

#include <stdbool.h>
#include <arch/delay.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/gpio.h>
//#include <common/i2c.h>
#include <ec/adc.h>
#include <ec/i2c.h>
#include <common/debug.h>

#define BAT_GAS_GAUGE_ADDR (0x16 >> 1)
#define BAT_EEPROM_ADR (0xa0 >> 1)


static uint16_t last_voltage_read = 0;
static bool sbs_battery = false;

//
// returns battery charge in % (0-100)
// based on last battery readings
//
uint16_t board_battery_get_charge(void)
{
uint16_t tvol;
uint16_t bchgst;
unsigned char ravg=0; // rolling average
static unsigned char ravg2=0;
static unsigned char ravg3=0;

    if (sbs_battery) {
        int res;
        res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0D, &ravg, 2);
        if (res < 0) {
            DEBUG(" 0x0D r=%d\n", res);
        }
        res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0F, &bchgst, 2);
        if (res < 0) {
            DEBUG(" 0x0D r=%d\n", res);
        } else
            battery_remaining_capacity = bchgst;
    } else {
        // if no design voltage is defined we can not determine anything,
        // bail out
        if (battery_design_voltage == 0)
            return 0;

        tvol=last_voltage_read;
        tvol -= battery_min_voltage; // lower threshold, = 0%
        if (gpio_get(&ACIN_N)) {
            // unplugged
            bchgst = ((30000 / (battery_design_voltage-battery_min_voltage)) * tvol) / 300;
        } else {
            // plugged
            bchgst = ((30000 / (battery_charge_voltage-battery_min_voltage)) * tvol) / 300;
        }

        if (bchgst > 100)
            bchgst = 100;

        ravg3 = ravg2;
        ravg2 = bchgst;
        ravg = (ravg2 + ravg3 + bchgst) / 3;

        battery_remaining_capacity = (battery_design_capacity / 100) * ravg; // guess based on voltage
    }

    return ravg; // in % from 0 to 100 - hopefully
}

//
// reads current battery voltage in mV
// in Librem 14 from EC ADC channel 1
//
uint16_t board_battery_get_voltage(void)
{
uint8_t to=0;
uint16_t adcval;
uint16_t tvol;

    if (sbs_battery) {
        int res;
        res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x09, &tvol, 2);
        if (res < 0) {
            DEBUG(" 0x09 r=%d\n", res);
        }
        //DEBUG("SBS %dmV\n", tvol);
    } else {
        // we may have to wait for ADC to finish
        while (to++ < 100 && !(VCH0CTL & (1L << 7)))
            delay_us(100);

        if (!(VCH0CTL & (1L << 7)))
            DEBUG("BAT !adc sts=0x%02x ctl=0x%02x\n", ADCSTS, VCH0CTL);

        adcval = (((uint16_t)VCH0DATM & 0x03) << 8) | VCH0DATL;
        // max bat voltage at max ADC value = 15.4V
        tvol = ((14000000 / 0x3ff) * adcval) / 1000;

        // clear ADC to start new cycle
        VCH0CTL |= (1L << 7);

        last_voltage_read = tvol;

        //DEBUG("ADC %dmV\n", tvol);
    }

    return tvol;
}

//
// reads charge / discharge current
// in Librem 14 from EC ADC channel 0
// sense resistor 0.01 Ohm,
// charging: IOUT x 40
// discharging: IOUT x 16
// EC ADC 10 bit 3Vmax
// 3V / 0x3ff * ADC = VADC
// (VADC / 0.01) / 40 = Icharge
// (VADC / 0.01) / 16 = Icharge
//
uint16_t board_battery_get_current(void)
{
uint8_t to=0;
uint16_t adcval;

    if (sbs_battery) {
        int res;
        res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0A, &adcval, 2);
        if (res < 0) {
            DEBUG(" 0x0A r=%d\n", res);
        }
        //DEBUG("SBS %dmA\n", adcval);
    } else {
        // we may have to wait for ADC to finish
        while (to++ < 100 && !(VCH1CTL & (1L << 7)))
            delay_us(100);

        if (!(VCH1CTL & (1L << 7)))
            DEBUG("CUR !adc sts=0x%02x ctl=0x%02x\n", ADCSTS, VCH1CTL);

        adcval = (((uint16_t)VCH1DATM & 0x03) << 8) | VCH1DATL;

        // DEBUG("cur raw: %d ", adcval);

        // clear ADC to start new cycle
        VCH1CTL |= (1L << 7);

        adcval = adcval * (30000 / 0x3ff); // = sense voltage
        if (gpio_get(&ACIN_N))
            adcval = (adcval / (16)) * 10;	// no AC, discharge current
        else
            adcval = adcval / (4);		// on AC, charge current

        //DEBUG("ADC %dmA\n", adcval);
    }

    battery_current = adcval;

    return battery_current;
}


static bool read_eeprom(void)
{
int res;
uint8_t vendor_id;
uint8_t cell_brand_id;
uint8_t battery_pack_S;
uint8_t battery_pack_T;
uint16_t charge_voltage;
uint16_t charge_current;
uint16_t design_capacity;
uint16_t design_voltage;
uint16_t max_discharge_current;

    // first read is a probe, if this fails there is no EEPROM
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x00, &vendor_id, 1);
    if (res < 0) {
        DEBUG("bat eep r=%d\n", res);
        return false;
    }
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x01, &cell_brand_id, 1);
    if (res < 0)
        cell_brand_id=0;
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x02, &battery_pack_S, 1);
    if (res < 0)
        battery_pack_S=0;
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x03, &battery_pack_T, 1);
    if (res < 0)
        battery_pack_T=0;
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x04, &charge_voltage, 2);
    if (res < 0)
        charge_voltage=0;
    else
        battery_charge_voltage=charge_voltage;
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x06, &design_capacity, 2);
    if (res < 0)
        design_capacity=0;
    else {
        battery_design_capacity = design_capacity;
        battery_full_capacity = design_capacity; // we can not determine anything else
    }
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x08, &design_voltage, 2);
    if (res < 0)
        design_voltage=0;
    else
        battery_design_voltage = design_voltage;
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x0A, &max_discharge_current, 2);
    if (res < 0)
        max_discharge_current=0;
    res = i2c_get(&I2C_0, BAT_EEPROM_ADR, 0x0C, &charge_current, 2);
    if (res < 0)
        charge_current = 0;
    else
        battery_charge_current = charge_current;

    DEBUG("  VID:      0x%02x\n", vendor_id);
    DEBUG("  cell ID:  0x%02x\n", cell_brand_id);
    DEBUG("  pack-S:   0x%02x\n", battery_pack_S);
    DEBUG("  pack-T:   0x%02x\n", battery_pack_T);
    DEBUG("  chg-volt: %dmV\n", charge_voltage);
    DEBUG("  chg-cur:  %dmA\n", charge_current);
    DEBUG("  des-cap:  %d\n", design_capacity);
    DEBUG("  des-volt: %dmV\n", design_voltage);
    DEBUG("  dis-cur:  %dmA\n", max_discharge_current);

    battery_temp = 20; // no thermistor, assume 20C ?
    battery_min_voltage = 9600;

    battery_current = 0; // we have no means to tell the current/rate

    return true;
}

static void update_gas_gauge(void)
{
int res;
int16_t tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x09, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x09 r=%d\n", res);
    }
    battery_voltage = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x08, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x08 r=%d\n", res);
    }
    battery_temp = (tval - 2731);

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0A, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x0A r=%d\n", res);
    }
    battery_current = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0D, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x0D r=%d\n", res);
    }
    battery_charge = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0F, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x0F r=%d\n", res);
    }
    battery_remaining_capacity = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x10, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x10 r=%d\n", res);
    }
    battery_full_capacity = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x17, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x17 r=%d\n", res);
    }
    battery_cycle_count = tval;
}

static bool probe_gas_gauge(void)
{
int res;
int16_t tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x00, &tval, 1);
    if (res < 0) {
        DEBUG("bat gauge r=%d\n", res);
        return false;
    }

    DEBUG("bat gauge:\n");
    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x09, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x09 r=%d\n", res);
    }
    DEBUG(" %dmV\n", tval);
    battery_voltage = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x08, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x08 r=%d\n", res);
    }
    DEBUG(" temp: %d\n", tval);
    battery_temp = (tval - 2731);

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0A, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x0A r=%d\n", res);
    }
    DEBUG(" %dmA\n", tval);
    battery_current = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0D, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x0D r=%d\n", res);
    }
    DEBUG(" %d%%\n", tval);
    battery_charge = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x0F, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x0F r=%d\n", res);
    }
    DEBUG(" rem cap: %d\n", tval);
    battery_remaining_capacity = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x10, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x10 r=%d\n", res);
    }
    DEBUG(" full cap: %d\n", tval);
    battery_full_capacity = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x14, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x14 r=%d\n", res);
    }
    DEBUG(" chg cur: %d\n", tval);

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x15, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x15 r=%d\n", res);
    }
    DEBUG(" chg volt: %d\n", tval);
    if (tval < 7000)
        battery_charge_voltage = 8700;
    else
        battery_charge_voltage = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x16, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x16 r=%d\n", res);
    }
    DEBUG(" stat: 0x%04x\n", tval);
    // battery_status = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x17, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x17 r=%d\n", res);
    }
    DEBUG(" cycle#: %d\n", tval);
    battery_cycle_count = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x18, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x18 r=%d\n", res);
    }
    DEBUG(" des-cap: %d\n", tval);
    battery_design_capacity = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x19, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x19 r=%d\n", res);
    }
    DEBUG(" des-volt: %d\n", tval);
    battery_design_voltage = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x1b, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x1b r=%d\n", res);
    }
    DEBUG(" man date: 0x%04x\n", tval);
    battery_manufacturing_date = tval;

    res = i2c_get(&I2C_0, BAT_GAS_GAUGE_ADDR, 0x1c, &tval, 2);
    if (res < 0) {
        DEBUG(" 0x1c r=%d\n", res);
    }
    DEBUG(" serial#: %d\n", tval);

    return true;
}

void board_battery_update_state(void)
{
    if (sbs_battery)
        update_gas_gauge();
    else {
        battery_voltage = board_battery_get_voltage();
        battery_current = board_battery_get_current();
        battery_charge = board_battery_get_charge();
    }
}

// try to figure out which battery we have
void board_battery_init(void)
{
    battery_voltage = 0;
    battery_temp = 0;
    battery_current = 0;
    // battery_charge = 100;
    battery_remaining_capacity = 0;
    battery_full_capacity = 0;
    battery_design_capacity = 0;
    battery_design_voltage = 0;
    battery_status &= ~BATTERY_INITIALIZED;

    battery_charger_disable();
    // charger_input_current = 1024; // about 1A @ 19V ~= 20W
    charger_input_current = 3420; // max current of 65W charger

    gpio_set(&CHG_CELL_CFG, false);

    battery_present = !gpio_get(&BAT_DETECT_N);

    if (battery_present) {
        battery_voltage = board_battery_get_voltage();
        if (read_eeprom()) {
            DEBUG("I: 3-cell bat found\n");
            // gpio_set(&CHG_CELL_CFG, false);
            battery_status |= BATTERY_INITIALIZED;
            sbs_battery = false;

            // charger_input_current = 0x1100;
            charger_min_system_voltage = 0x1E00;

            battery_charger_enable();
            battery_charger_disable();
        } else {
            if (probe_gas_gauge()) {
                DEBUG("I: 4-cell bat found\n");
                // gpio_set(&CHG_CELL_CFG, true);

                // battery_temp = 0;
                // battery_current = 0;
                // battery_charge = 0;
                // battery_remaining_capacity = 0;
                // battery_full_capacity = 8800;
                // battery_design_capacity = 8800;
                // battery_design_voltage = 7600;
                battery_charge_current = 1760;
                // battery_charge_voltage = 8700;
                battery_min_voltage = 6000;

                // charger_input_current = 0x1100;
                charger_min_system_voltage = 0x1600;

                battery_status |= BATTERY_INITIALIZED;

                sbs_battery = true;
                battery_charger_enable();
                battery_charger_disable();
            } else {
                DEBUG("E: unknown bat, keeping charger off!\n");
                battery_status &= ~BATTERY_INITIALIZED;
                // gpio_set(&CHG_CELL_CFG, true);
                sbs_battery = false;
            }
        }
    }
}
