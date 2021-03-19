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
#endif // _BOARD_BOARD_H
