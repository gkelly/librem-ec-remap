// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_BOARD_H
#define _BOARD_BOARD_H

#include <stdbool.h>

void board_init(void);
void board_event(void);
void board_1s_event(void);
void board_on_ac(bool ac);

void board_battery_init(void);

// voltage in mV
uint16_t board_battery_get_voltage(void);
// current in mA
uint16_t board_battery_get_current(void);
// charge in % (0 to 100)
uint16_t board_battery_get_charge(void);

void board_battery_update_state(void);


// persistent settings stored in BRAM bank#1 @ 0x80..0xbf
#define BRAM_OFFSET 0x80
#define BRAM_CHARGE_START_THRES         (BRAM_OFFSET + 0)
#define BRAM_CHARGE_END_THRES           (BRAM_OFFSET + 1)

bool bram_init(void);
bool bram_set_value(uint8_t offset, uint8_t value);

#endif // _BOARD_BOARD_H
