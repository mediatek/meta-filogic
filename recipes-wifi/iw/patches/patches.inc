#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://010-Revert-iw-allow-specifying-CFLAGS-LIBS-externally.patch \
    file://1000-Add-support-for-5G-U-NII-3-6G-BW320.patch \
    file://130-survey-bss-rx-time.patch \
    "
