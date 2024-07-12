#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-iw-refactor-frequency-help.patch \
    file://0002-iw-add-puncturing-support.patch \
    file://0003-util-clarify-comment-about-parsed-pointer.patch \
    file://0004-iw-remove-sizer-section-and-related-code.patch \
    file://0005-Revert-iw-allow-specifying-CFLAGS-LIBS-externally.patch \
    file://0006-survey-bss-rx-time.patch \
    file://0007-iw-support-link-id-in-set-bitrates-command.patch \
    file://0008-iw-add-per-link-txpower-config.patch \
    file://0009-mtk-wifi-iw-dump-links-information-in-station-dump.patch \
    file://0010-iw-add-per-radio-antenna-config.patch \
    "
