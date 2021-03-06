# SPDX-License-Identifier: GPL-3.0-only

# Set log level
# 0 - NONE
# 1 - ERROR
# 2 - WARN
# 3 - INFO
# 4 - DEBUG
# 5 - TRACE
CFLAGS+=-DLEVEL=4

# Uncomment to enable debug logging over keyboard parallel port
#CFLAGS+=-DPARALLEL_DEBUG

# Uncomment to enable I2C debug on 0x76
#CFLAGS+=-DI2C_DEBUGGER=0x76

# Set external programmer
PROGRAMMER=$(wildcard /dev/serial/by-id/usb-Arduino*)

# Include purism common source
PURISM_COMMON_DIR=src/board/purism/common
SRC+=$(wildcard $(PURISM_COMMON_DIR)/*.c)
INCLUDE+=$(wildcard $(PURISM_COMMON_DIR)/include/common/*.h) $(PURISM_COMMON_DIR)/common.mk
CFLAGS+=-I$(PURISM_COMMON_DIR)/include

# Add charger
CHARGER?=bq24715
SRC+=$(PURISM_COMMON_DIR)/charger/$(CHARGER).c

# Add kbled
KBLED?=none
SRC+=$(PURISM_COMMON_DIR)/kbled/$(KBLED).c

# Add scratch ROM
include $(PURISM_COMMON_DIR)/scratch/scratch.mk

# Add scratch ROM for flash access
include $(PURISM_COMMON_DIR)/flash/flash.mk

console_internal:
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool console

console_external:
	sudo test -c "$(PROGRAMMER)"
	sleep 1 && echo C | sudo tee "$(PROGRAMMER)" &
	sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"

flash_internal: $(BUILD)/ec.rom
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool flash $<

flash_external: $(BUILD)/ec.rom
	cargo build --manifest-path ecflash/Cargo.toml --example isp --release
	sudo ecflash/target/release/examples/isp $<
