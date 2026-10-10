# SPDX-License-Identifier: GPL-2.0-only
#
# Out-of-tree build of the MediaTek MT7668 (SDIO) Bluetooth driver:
# btmtksdio and btmtk from linux-6.18.y drivers/bluetooth with the MT7668
# changes. ../compat/bluetooth/ holds everything needed to build them on
# older kernels.
#

ifneq ($(KERNELRELEASE),)

BT_SRC := $(abspath $(if $(filter /%,$(src)),$(src),$(srctree)/$(src)))
BT_COMPAT := $(BT_SRC)/../compat/bluetooth

obj-m += btmtk.o btmtksdio.o

# compat.h is force-included into every object; hci_uart.h and
# linux/unaligned.h are found through the include paths.
ccflags-y += -I$(BT_COMPAT) -I$(BT_COMPAT)/include \
	     -include $(BT_COMPAT)/compat.h

else

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
M ?= $(shell pwd)

.PHONY: default modules modules_install clean

default: modules

modules modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@

endif
