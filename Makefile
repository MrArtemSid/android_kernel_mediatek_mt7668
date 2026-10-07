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

# Wi-Fi: wlan/Makefile is the upstream Kbuild file.
export CONFIG_MT7668 := m
obj-m += wlan/

# compat.h is force-included into every wlan/ object, so the driver
# sources themselves never need to know which kernel they build for.
subdir-ccflags-y += -include $(MT7668_ROOT)/compat/compat.h

# wlan/Makefile adds its include paths as -I$(src)/..., which only works
# since Linux 6.10 made $(src) absolute. Add them absolute here as well.
MT7668_WLAN := $(MT7668_ROOT)/wlan
subdir-ccflags-y += -I$(MT7668_WLAN)/os -I$(MT7668_WLAN)/os/linux/include \
		    -I$(MT7668_WLAN)/include -I$(MT7668_WLAN)/include/nic \
		    -I$(MT7668_WLAN)/include/mgmt -I$(MT7668_WLAN)/include/chips \
		    -I$(MT7668_WLAN)/os/linux/hif/sdio/include

# The kernel builds with -Werror; keep the vendor code's warnings visible
# without failing the build on them.
subdir-ccflags-y += -Wno-error=sometimes-uninitialized \
		    -Wno-error=non-literal-null-conversion \
		    -Wno-error=tautological-overlap-compare \
		    -Wno-error=parentheses-equality \
		    -Wno-error=misleading-indentation \
		    -Wno-error=unneeded-internal-declaration

else

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
M ?= $(shell pwd)

.PHONY: default modules modules_install clean

default: modules

modules modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@

endif
