#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-wifi-mt76-mt7996-add-eht-rx-rate-support.patch \
    file://0002-wifi-mt76-mt7996-reduce-repeated-bss_info-and-sta_re.patch \
    file://0003-wifi-mt76-mt7996-move-radio-enable-command-to-mt7996.patch \
    file://0004-wifi-mt76-connac-set-correct-muar_idx-for-connac3-ch.patch \
    file://0005-wifi-mt76-mt7996-add-muru-support.patch \
    file://0006-wifi-mt76-mt7996-set-txd-v1.patch \
    file://0007-wifi-mt76-mt7996-add-thermal-protection-support.patch \
    file://0008-wifi-mt76-mt7996-add-thermal-sensor-device-support.patch \
    file://0009-wifi-mt76-mt7996-add-dsp-firmware-download.patch \
    file://0010-wifi-mt76-mt7996-fix-icv-error-when-enable-AP-and-ST.patch \
    file://0011-wifi-mt76-mt7996-set-wcid-in-txp.patch \
    file://0012-wifi-mt76-mt7996-init-he-and-eht-cap-for-AP_VLAN.patch \
    file://0013-wifi-mt76-mt7996-fix-beamform-mcu-cmd-configuration.patch \
    file://0014-wifi-mt76-mt7996-Fix-using-the-wrong-phy-for-backgro.patch \
    file://0015-wifi-mt76-mt7996-support-more-options-in-.set_bitrat.patch \
    file://0016-wifi-mt76-mt7996-fill-txwi-by-SW-temporarily.patch \
    file://0017-wifi-mt76-mt7996-update-wmm-queue-mapping.patch \
    file://0018-wifi-mt76-mt7996-enable-IDS-debug-log.patch \
    file://0019-mt76-testmode-add-atenl-support-in-mt7996.patch \
    file://0020-mt76-testmode-add-basic-testmode-support.patch \
    file://0021-mt76-testmode-add-chainmask-hacking-for-eagle-band-2.patch \
    file://0022-mt76-revert-page-pool-changes.patch \
    file://0999-mt76-mt7996-for-build-pass.patch \
    file://1000-mt76-mt7996-add-debug-tool.patch \
    file://1001-mt76-mt7996-add-txpower-support.patch \
    "
