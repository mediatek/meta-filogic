#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI:append = " \
    file://0001-iw-jump-to-2025-10-27-codebase.patch \
    file://0002-openwrt-Revert-iw-allow-specifying-CFLAGS-LIBS-exter.patch \
    file://0003-openwrt-survey-bss-rx-time.patch \
    file://0004-openwrt-vif-radio-mask.patch \
    file://0005-mtk-sync-nl80211.h.patch \
    file://0006-iw-support-link-id-in-set-bitrates-command.patch \
    file://0007-iw-add-link_id-for-offchannel-operation.patch \
    file://0008-mtk-iw-dump-per-radio-antenna_mask.patch \
    file://0009-iw-add-radio-mask-for-scanning.patch \
    file://0010-mtk-iw-support-per-radio-txpower-setting.patch \
    file://0011-mtk-iw-allow-to-specify-radio_idx-when-setting-TX-RX.patch \
    "
