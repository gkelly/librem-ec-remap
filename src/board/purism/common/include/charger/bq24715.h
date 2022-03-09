// SPDX-License-Identifier: GPL-3.0-only
//
// TI BQ24715 Smart Charger
// https://www.ti.com/lit/gpn/BQ24715

#ifndef _CHARGER_BQ24715_H
#define _CHARGER_BQ24715_H

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

#endif
