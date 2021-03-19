// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <ec/pwm.h>
#include <common/debug.h>


void kbled_init(void) {
    DEBUG("kbled_init\n");
    //TODO: enable PWMs
    kbled_reset();
}

void kbled_reset(void) {
    DEBUG("kbled_reset\n");
    // Set default brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x00);
}

uint8_t kbled_get(void) {
    DEBUG("kbled_get()=%d\n", DCR6);
    // Get PWM for power
    return DCR6;
}

uint8_t kbled_max(void) {
    DEBUG("kbled_max()=%d\n", 0xff);
    return 0xff;
}

void kbled_set(uint8_t level) {
    // Set PWM for power
    DEBUG("kbled_set(%d)\n", level);
    DCR6 = level;
}

uint32_t kbled_get_color(void) {
    DEBUG("kbled_get_color()=0x%06x\n", 0xffffff);
    return 0xffffff;
}

void kbled_set_color(uint32_t color) {
    DEBUG("kbled_set_color(0x%06x)\n", color);
    color = color;
    return;
}
