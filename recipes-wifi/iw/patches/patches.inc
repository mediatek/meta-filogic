#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI:append = " \
    file://001-nl80211_h_sync.patch \
    file://010-Revert-iw-allow-specifying-CFLAGS-LIBS-externally.patch \
    file://1000-Add-support-for-5G-U-NII-3-6G-BW320.patch \
    file://130-survey-bss-rx-time.patch \
    file://300-wiphy_radios.patch \
    file://310-vif_radio_mask.patch \
    "
