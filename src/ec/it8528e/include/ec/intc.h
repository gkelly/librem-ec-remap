// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_INTC_H
#define _EC_INTC_H

#include <stdint.h>

#define INTC_BASE 0x1100

// interrupt status register
volatile uint8_t __xdata __at(INTC_BASE + 0x00) ISR0;
volatile uint8_t __xdata __at(INTC_BASE + 0x01) ISR1;
volatile uint8_t __xdata __at(INTC_BASE + 0x02) ISR2;
volatile uint8_t __xdata __at(INTC_BASE + 0x03) ISR3;
volatile uint8_t __xdata __at(INTC_BASE + 0x14) ISR4;
volatile uint8_t __xdata __at(INTC_BASE + 0x18) ISR5;
volatile uint8_t __xdata __at(INTC_BASE + 0x1c) ISR6;
volatile uint8_t __xdata __at(INTC_BASE + 0x20) ISR7;
volatile uint8_t __xdata __at(INTC_BASE + 0x24) ISR8;
volatile uint8_t __xdata __at(INTC_BASE + 0x28) ISR9;
volatile uint8_t __xdata __at(INTC_BASE + 0x2c) ISR10;
volatile uint8_t __xdata __at(INTC_BASE + 0x30) ISR11;
volatile uint8_t __xdata __at(INTC_BASE + 0x34) ISR12;
volatile uint8_t __xdata __at(INTC_BASE + 0x38) ISR13;
volatile uint8_t __xdata __at(INTC_BASE + 0x3c) ISR14;
volatile uint8_t __xdata __at(INTC_BASE + 0x40) ISR15;
volatile uint8_t __xdata __at(INTC_BASE + 0x44) ISR16;
volatile uint8_t __xdata __at(INTC_BASE + 0x48) ISR17;
volatile uint8_t __xdata __at(INTC_BASE + 0x4c) ISR18;
// interrupt enable register
volatile uint8_t __xdata __at(INTC_BASE + 0x04) IER0;
volatile uint8_t __xdata __at(INTC_BASE + 0x05) IER1;
volatile uint8_t __xdata __at(INTC_BASE + 0x06) IER2;
volatile uint8_t __xdata __at(INTC_BASE + 0x07) IER3;
volatile uint8_t __xdata __at(INTC_BASE + 0x15) IER4;
volatile uint8_t __xdata __at(INTC_BASE + 0x19) IER5;
volatile uint8_t __xdata __at(INTC_BASE + 0x1d) IER6;
volatile uint8_t __xdata __at(INTC_BASE + 0x21) IER7;
volatile uint8_t __xdata __at(INTC_BASE + 0x25) IER8;
volatile uint8_t __xdata __at(INTC_BASE + 0x29) IER9;
volatile uint8_t __xdata __at(INTC_BASE + 0x2d) IER10;
volatile uint8_t __xdata __at(INTC_BASE + 0x31) IER11;
volatile uint8_t __xdata __at(INTC_BASE + 0x35) IER12;
volatile uint8_t __xdata __at(INTC_BASE + 0x39) IER13;
volatile uint8_t __xdata __at(INTC_BASE + 0x3d) IER14;
volatile uint8_t __xdata __at(INTC_BASE + 0x41) IER15;
volatile uint8_t __xdata __at(INTC_BASE + 0x45) IER16;
volatile uint8_t __xdata __at(INTC_BASE + 0x49) IER17;
volatile uint8_t __xdata __at(INTC_BASE + 0x4d) IER18;
// Interrupt Edge/Level-Triggered Mode Register
volatile uint8_t __xdata __at(INTC_BASE + 0x08) IELMR0;
volatile uint8_t __xdata __at(INTC_BASE + 0x09) IELMR1;
volatile uint8_t __xdata __at(INTC_BASE + 0x0a) IELMR2;
volatile uint8_t __xdata __at(INTC_BASE + 0x0b) IELMR3;
volatile uint8_t __xdata __at(INTC_BASE + 0x16) IELMR4;
volatile uint8_t __xdata __at(INTC_BASE + 0x1a) IELMR5;
volatile uint8_t __xdata __at(INTC_BASE + 0x1e) IELMR6;
volatile uint8_t __xdata __at(INTC_BASE + 0x22) IELMR7;
volatile uint8_t __xdata __at(INTC_BASE + 0x26) IELMR8;
volatile uint8_t __xdata __at(INTC_BASE + 0x2a) IELMR9;
volatile uint8_t __xdata __at(INTC_BASE + 0x2e) IELMR10;
volatile uint8_t __xdata __at(INTC_BASE + 0x32) IELMR11;
volatile uint8_t __xdata __at(INTC_BASE + 0x36) IELMR12;
volatile uint8_t __xdata __at(INTC_BASE + 0x3a) IELMR13;
volatile uint8_t __xdata __at(INTC_BASE + 0x3e) IELMR14;
volatile uint8_t __xdata __at(INTC_BASE + 0x42) IELMR15;
volatile uint8_t __xdata __at(INTC_BASE + 0x46) IELMR16;
volatile uint8_t __xdata __at(INTC_BASE + 0x4a) IELMR17;
volatile uint8_t __xdata __at(INTC_BASE + 0x4e) IELMR18;
// Interrupt Polarity Register
volatile uint8_t __xdata __at(INTC_BASE + 0x0c) IPOLR0;
volatile uint8_t __xdata __at(INTC_BASE + 0x0d) IPOLR1;
volatile uint8_t __xdata __at(INTC_BASE + 0x0e) IPOLR2;
volatile uint8_t __xdata __at(INTC_BASE + 0x0f) IPOLR3;
volatile uint8_t __xdata __at(INTC_BASE + 0x17) IPOLR4;
volatile uint8_t __xdata __at(INTC_BASE + 0x1b) IPOLR5;
volatile uint8_t __xdata __at(INTC_BASE + 0x1f) IPOLR6;
volatile uint8_t __xdata __at(INTC_BASE + 0x23) IPOLR7;
volatile uint8_t __xdata __at(INTC_BASE + 0x27) IPOLR8;
volatile uint8_t __xdata __at(INTC_BASE + 0x2b) IPOLR9;
volatile uint8_t __xdata __at(INTC_BASE + 0x2f) IPOLR10;
volatile uint8_t __xdata __at(INTC_BASE + 0x33) IPOLR11;
volatile uint8_t __xdata __at(INTC_BASE + 0x37) IPOLR12;
volatile uint8_t __xdata __at(INTC_BASE + 0x3b) IPOLR13;
volatile uint8_t __xdata __at(INTC_BASE + 0x3f) IPOLR14;
volatile uint8_t __xdata __at(INTC_BASE + 0x43) IPOLR15;
volatile uint8_t __xdata __at(INTC_BASE + 0x47) IPOLR16;
volatile uint8_t __xdata __at(INTC_BASE + 0x4b) IPOLR17;
volatile uint8_t __xdata __at(INTC_BASE + 0x4f) IPOLR18;
// Interrupt Vector Register
volatile uint8_t __xdata __at(INTC_BASE + 0x10) IVECT;
// INT0# status
volatile uint8_t __xdata __at(INTC_BASE + 0x11) INT0ST;
// Power Fail Register
volatile uint8_t __xdata __at(INTC_BASE + 0x12) PFAILR;

#undef INTC_BASE
#endif
