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
