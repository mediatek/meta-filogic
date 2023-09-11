#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://010-Revert-iw-allow-specifying-CFLAGS-LIBS-externally.patch \
    file://1000-Add-support-to-set-control-and-center-freq-for-6G-BW.patch \
    file://130-survey-bss-rx-time.patch \
    "
