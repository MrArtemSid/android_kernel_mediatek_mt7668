load("//build/kernel/kleaf:kernel.bzl", "kernel_module")

# MediaTek MT7668 (SDIO) Wi-Fi: wlan_mt76x8_sdio.ko (SDIO function 1).
def mt7668_module(name, kernel_build, deps = None):
    kernel_module(
        name = name,
        srcs = ["//vendor/mediatek/mt7668:mt7668_srcs"],
        makefile = ["//vendor/mediatek/mt7668:Makefile"],
        deps = deps,
        outs = [
            "wlan_mt76x8_sdio.ko",
        ],
        kernel_build = kernel_build,
    )

# MediaTek MT7668 (SDIO) Bluetooth: btmtksdio.ko (SDIO function 2) and btmtk.ko,
# from linux-6.18.y. They replace the in-tree btmtksdio, so drop that one
# from module_outs; bluetooth.ko comes from the kernel build.
def mt7668_bt_module(name, kernel_build, deps = None):
    kernel_module(
        name = name,
        srcs = [
            "//vendor/mediatek/mt7668/bluetooth:mt7668_bt_srcs",
            "//vendor/mediatek/mt7668:mt7668_bt_compat_srcs",
        ],
        makefile = ["//vendor/mediatek/mt7668/bluetooth:Makefile"],
        deps = deps,
        outs = [
            "btmtk.ko",
            "btmtksdio.ko",
        ],
        kernel_build = kernel_build,
    )
