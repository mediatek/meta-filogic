#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-iw-info-print-PMSR-capabilities.patch \
    file://0002-iw-add-cac-background-command.patch \
    file://0003-iw-info-fix-bug-reading-preambles-and-bandwidths.patch \
    file://0004-iw-add-support-for-retrieving-keys.patch \
    file://0005-iw-event-fix-printf-format-error.patch \
    file://0006-update-nl80211.h.patch \
    file://0007-util-add-support-for-320Mhz-bandwidth.patch \
    file://0008-util-add-support-for-320MHz-bandwidth-without-cf1.patch \
    file://0009-iw-scan-set-NL80211_SCAN_FLAG_COLOCATED_6GHZ-in-case.patch \
    file://0010-link-fix-some-formatting.patch \
    file://0011-link-update-for-MLO.patch \
    file://0012-interface-print-links.patch \
    file://0013-util-don-t-print-EHT-info-if-not-present.patch \
    file://0014-iw-S1G-add-frequency-set-in-kHz-and-offset-options.patch \
    file://0015-iw-S1G-add-parsing-for-802.11ah-scan-IE-s.patch \
    file://0016-iw-S1G-add-list-command-support-for-802.11ah.patch \
    file://0017-iw-Fix-EHT-rates-printing.patch \
    file://0018-iw-add-more-extended-capa-bits.patch \
    file://0019-iw-fix-attribute-size-mismatch.patch \
    file://0020-iw-connect-Fix-segfault-during-open-authentication.patch \
    file://0021-update-nl80211.h.patch \
    file://0022-iw-S1G-add-802.11ah-support-for-link-command-display.patch \
    file://0023-iw-allow-extra-cflags.patch \
    file://0024-update-nl80211.h.patch \
    file://0025-bump-version-to-6.7.patch \
    file://0026-update-nl80211.h.patch \
    file://0027-Revert-iw-allow-specifying-CFLAGS-LIBS-externally.patch \
    file://0028-survey-bss-rx-time.patch \
    "
