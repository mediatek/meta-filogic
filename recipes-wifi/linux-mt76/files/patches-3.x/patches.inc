#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-wifi-mt76-mt7996-fix-chainmask-calculation-in-mt7996.patch \
    file://0002-wifi-mt76-mt7996-update-register-for-CFEND_RATE.patch \
    file://0003-wifi-mt76-mt7996-do-not-hardcode-vht-beamform-cap.patch \
    file://0004-wifi-mt76-connac-fix-POWER_CTRL-command-name-typo.patch \
    file://4001-mt76-mt7996-for-build-pass.patch \
    file://4002-mt76-revert-get_of_mac_addr.patch \
    file://4003-mt76-include-header-files-for-module-param.patch \
    "
