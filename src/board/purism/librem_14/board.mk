# SPDX-License-Identifier: GPL-3.0-only

EC=it8528e

# Add keymap to src
KEYMAP?=default
SRC+=$(BOARD_DIR)/keymap/$(KEYMAP).c

# Set keyboard LED mechanism
KBLED=white_gpio

# Set charger I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_3

# Set battery I2C bus
CFLAGS+=-DI2C_BATTERY=I2C_0

# Set I2C bus for debug output
CFLAGS+=-DI2C_DEBUG_BUS=I2C_0

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=0x0200 \
	-DCHARGER_CHARGE_VOLTAGE=0x3000 \
	-DCHARGER_INPUT_CURRENT=0x1100

# Set battery charging thresholds
CFLAGS+=\
	-DBATTERY_START_THRESHOLD=0 \
	-DBATTERY_END_THRESHOLD=100

CFLAGS+=-DHAVE_LID_SW

# Add purism common code
include src/board/purism/common/common.mk
