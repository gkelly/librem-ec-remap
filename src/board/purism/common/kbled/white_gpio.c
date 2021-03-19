// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <board/gpio.h>
#include <common/debug.h>


void kbled_init(void) {
    //DEBUG("kbled_init\n");
    kbled_reset();
}

void kbled_reset(void) {
    //DEBUG("kbled_reset\n");
    // Set default brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x01); // on by default
}

uint8_t kbled_get(void) {
    //DEBUG("kbled_get()=%d\n", gpio_get(&KBD_BACKLIGHT_EN));
    return gpio_get(&KBD_BACKLIGHT_EN);
}

uint8_t kbled_max(void) {
    //DEBUG("kbled_max()=%d\n", 0xff);
    return 0x01; // there is only on and off
}

void kbled_set(uint8_t level) {
    //DEBUG("kbled_set(%d)\n", level);
    if (level > kbled_max())
        level = 0;
    gpio_set(&KBD_BACKLIGHT_EN, level);
}

uint32_t kbled_get_color(void) {
    //DEBUG("kbled_get_color()=0x%06x\n", 0xffffff);
    return 0xffffff;
}

void kbled_set_color(uint32_t color) {
    //DEBUG("kbled_set_color(0x%06x)\n", color);
    color = color;
}
