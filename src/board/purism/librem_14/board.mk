# SPDX-License-Identifier: GPL-3.0-only

EC=it8528e

# Add keymap to src
KEYMAP?=default
SRC+=$(BOARD_DIR)/keymap/$(KEYMAP).c

# Set keyboard LED mechanism
KBLED=white_pwm

# Set charger I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_3

# Set battery I2C bus
CFLAGS+=-DI2C_BATTERY=I2C_0

# Set I2C bus for debug output
CFLAGS+=-DI2C_DEBUG_BUS=I2C_0

CFLAGS+=-DHAVE_LID_SW

# Add purism common code
include src/board/purism/common/common.mk
