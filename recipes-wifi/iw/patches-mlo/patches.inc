#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-iw-refactor-frequency-help.patch \
    file://0002-iw-add-puncturing-support.patch \
    file://0003-util-clarify-comment-about-parsed-pointer.patch \
    file://0004-iw-remove-sizer-section-and-related-code.patch \
    file://0005-iw-fix-formats-under-MIPS64-PPC.patch \
    file://0006-update-nl80211.h.patch \
    file://0007-scan-Add-printing-of-HE-Operation-Element.patch \
    file://0008-iw-scan-add-enum-for-element-IDs.patch \
    file://0009-update-nl80211.h.patch \
    file://0010-iw-add-output-for-wiphy-interface-combinations.patch \
    file://0011-iw-print-tx-power-per-link-for-MLD.patch \
    file://0012-iw-scan-replace-passed-ie-buffer-with-ie-context.patch \
    file://0013-openwrt-Revert-iw-allow-specifying-CFLAGS-LIBS-exter.patch \
    file://0014-openwrt-survey-bss-rx-time.patch \
    file://0015-openwrt-vif-radio-mask.patch \
    file://0016-iw-support-link-id-in-set-bitrates-command.patch \
    file://0017-iw-add-per-link-txpower-config.patch \
    file://0018-mtk-wifi-iw-dump-links-information-in-station-dump.patch \
    file://0019-iw-add-link_id-for-offchannel-operation.patch \
    file://0020-mtk-iw-dump-per-radio-antenna_mask.patch \
    file://0021-iw-add-radio-mask-for-scanning.patch \
    "
