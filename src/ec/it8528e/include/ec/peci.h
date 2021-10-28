// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PECI_H
#define _EC_PECI_H

#include <common/macro.h>
#include <stdint.h>

static volatile uint8_t __xdata __at(0x3000) HOSTAR;
#define HOBY			BIT(0x00)
#define FINISH			BIT(0x01)
#define RD_FCS_ERR		BIT(0x02)
#define WR_FCS_ERR		BIT(0x03)
#define EXTERR			BIT(0x05)
#define BUSERR			BIT(0x06)
#define BUSERR			BIT(0x06)
#define RCV_ERRCODE		BIT(0x07)
static volatile uint8_t __xdata __at(0x3001) HOCTLR;
#define START			BIT(0x00)
#define AWFCS_EN		BIT(0x01)
#define PECIHEN			BIT(0x03)
#define FCSERR_ABT		BIT(0x04)
#define FIFOCLR			BIT(0x05)
static volatile uint8_t __xdata __at(0x3002) HOCMDR;
static volatile uint8_t __xdata __at(0x3003) HOTRADDR;
static volatile uint8_t __xdata __at(0x3004) HOWRLR;
static volatile uint8_t __xdata __at(0x3005) HORDLR;
static volatile uint8_t __xdata __at(0x3006) HOWRDR;
static volatile uint8_t __xdata __at(0x3007) HORDDR;
static volatile uint8_t __xdata __at(0x3008) HOCTL2R;
#define HOPTTRS_2MHZ	0x00
#define HOPTTRS_1MHZ	BIT(0x00)
#define HOPTTRS_1_6MHZ	BIT(0x01)
static volatile uint8_t __xdata __at(0x3009) RWFCSV;
static volatile uint8_t __xdata __at(0x300E) PADCTLR;
#define HOVTTS_1_10V	0x00
#define HOVTTS_1_05V	BIT(0x00)


#endif // _EC_PECI_H
