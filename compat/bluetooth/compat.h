/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Builds bluetooth/ (linux-6.18.y btmtksdio and btmtk) on older kernels.
 * Force-included into every bluetooth/ object, so the driver sources
 * themselves never need to know which kernel they build for.
 */
#ifndef MT7668_COMPAT_BLUETOOTH_H
#define MT7668_COMPAT_BLUETOOTH_H

#include <linux/version.h>

/* btmtk is built here as a module of its own, whatever the kernel config. */
#undef CONFIG_BT_MTK
#define CONFIG_BT_MTK_MODULE 1

/* Only the SDIO driver is built: leave out btmtk's USB support. */
#undef CONFIG_BT_HCIBTUSB_MTK
#undef CONFIG_BT_HCIBTUSB_MTK_MODULE

#include <linux/device.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/pm_wakeup.h>

#ifndef SDIO_DEVICE_ID_MEDIATEK_MT7961
#define SDIO_DEVICE_ID_MEDIATEK_MT7961		0x7961
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 17, 0)
#define hci_set_quirk(hdev, nr)		set_bit((nr), &(hdev)->quirks)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 14, 0)
static void mt7668_compat_disable_wakeup(void *dev)
{
	device_init_wakeup(dev, false);
}

static inline int devm_device_init_wakeup(struct device *dev)
{
	int err = device_init_wakeup(dev, true);

	if (err)
		return err;
	return devm_add_action_or_reset(dev, mt7668_compat_disable_wakeup,
					dev);
}
#endif

/* hci_sync arrived in 5.17. Its only user here, btmtk_reset_sync(), runs on
 * a devcoredump completion, and devcoredump needs 6.4 (see btmtk.c).
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
struct hci_dev;

static inline int hci_cmd_sync_queue(struct hci_dev *hdev,
				     int (*func)(struct hci_dev *, void *),
				     void *data,
				     void (*destroy)(struct hci_dev *, void *,
						     int))
{
	return -EOPNOTSUPP;
}
#endif

#endif /* MT7668_COMPAT_BLUETOOTH_H */
