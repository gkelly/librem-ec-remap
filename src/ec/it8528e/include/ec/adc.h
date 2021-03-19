// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_ADC_H
#define _EC_ADC_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1900) ADCSTS;
volatile uint8_t __xdata __at(0x1901) ADCCFG;
volatile uint8_t __xdata __at(0x1902) ADCCTL;
volatile uint8_t __xdata __at(0x1903) ADCGCR;
volatile uint8_t __xdata __at(0x1904) VCH0CTL;
volatile uint8_t __xdata __at(0x1905) KDCTL;
volatile uint8_t __xdata __at(0x1906) VCH1CTL;
volatile uint8_t __xdata __at(0x1907) VCH1DATL;
volatile uint8_t __xdata __at(0x1908) VCH1DATM;
volatile uint8_t __xdata __at(0x1909) VCH2CTL;
volatile uint8_t __xdata __at(0x190A) VCH2DATL;
volatile uint8_t __xdata __at(0x190B) VCH2DATM;
volatile uint8_t __xdata __at(0x190C) VCH3CTL;
volatile uint8_t __xdata __at(0x190D) VCH3DATL;
volatile uint8_t __xdata __at(0x190E) VCH3DATM;
volatile uint8_t __xdata __at(0x1918) VCH0DATL;
volatile uint8_t __xdata __at(0x1919) VCH0DATM;
volatile uint8_t __xdata __at(0x1937) VCMPSCP;
volatile uint8_t __xdata __at(0x1938) VCH4CTL;
volatile uint8_t __xdata __at(0x1939) VCH4DATM;
volatile uint8_t __xdata __at(0x193A) VCH4DATL;
volatile uint8_t __xdata __at(0x193B) VCH5CTL;
volatile uint8_t __xdata __at(0x193C) VCH5DATM;
volatile uint8_t __xdata __at(0x193D) VCH5DATL;
volatile uint8_t __xdata __at(0x193E) VC6CTL;
volatile uint8_t __xdata __at(0x193F) VCH6DATM;
volatile uint8_t __xdata __at(0x1940) VCH6DATL;
volatile uint8_t __xdata __at(0x1941) VCH7CTL;
volatile uint8_t __xdata __at(0x1942) VCH7DATM;
volatile uint8_t __xdata __at(0x1943) VCH7DATL;
volatile uint8_t __xdata __at(0x1944) ADCDVSTS;
volatile uint8_t __xdata __at(0x1945) VCMPSTS;
volatile uint8_t __xdata __at(0x1946) VCMP0CTL;
volatile uint8_t __xdata __at(0x1947) VCMP0DATM;
volatile uint8_t __xdata __at(0x1948) VCMP0DATL;
volatile uint8_t __xdata __at(0x1949) VCMP1CTL;
volatile uint8_t __xdata __at(0x194A) VCMP1DATM;
volatile uint8_t __xdata __at(0x194B) VCMP1DATL;
volatile uint8_t __xdata __at(0x194C) VCMP2CTL;
volatile uint8_t __xdata __at(0x194D) VCMP2DATM;
volatile uint8_t __xdata __at(0x194E) VCMP2DATL;
volatile uint8_t __xdata __at(0x194F) VCMPOTR;
volatile uint8_t __xdata __at(0x1952) VCMPLR;

void adc_enable(bool enable);
void adc_init(void);

#endif // _EC_ADC_H
