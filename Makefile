# SPDX-License-Identifier: GPL-2.0-only
#
# Out-of-tree build of the MediaTek MT7668 (SDIO) Wi-Fi driver.
#
#   wlan/       MediaTek fullmac Wi-Fi driver, kept identical to
#               unifreq/linux-6.18.y drivers/net/wireless/mediatek/mt7668
#   compat/     everything needed to build wlan/ on older kernels
#

ifneq ($(KERNELRELEASE),)

MT7668_ROOT := $(abspath $(srctree)/$(src))

export CONFIG_MT7668 := m

# compat.h is force-included into every wlan/ object, so the driver
# sources themselves never need to know which kernel they build for.
subdir-ccflags-y += -include $(MT7668_ROOT)/compat/compat.h

obj-m += wlan/

else

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
M ?= $(shell pwd)

.PHONY: default modules modules_install clean

default: modules

modules modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@

endif
