#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI:append = " \
    file://010-Revert-iw-allow-specifying-CFLAGS-LIBS-externally.patch \
    file://130-survey-bss-rx-time.patch \
    file://310-vif_radio_mask.patch \
    "
