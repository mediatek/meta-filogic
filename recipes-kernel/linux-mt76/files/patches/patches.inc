#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-mt76-mt7915-rework-testmode-init-registers.patch \
    file://0002-mt76-testmode-rework-tx-antenna-setting.patch \
    file://0003-mt76-mt7915-rework-rx-testmode-stats.patch \
    file://0004-mt76-mt7915-fix-tx-descriptor.patch \
    file://0005-mt76-mt7915-fix-MBSS-index-condition-in-DBDC-mode.patch \
    file://0006-mt76-mt7915-support-VHT-MCS10-11.patch \
    file://0007-mt76-mt7915-update-mt7986-CR-for-different-adie-vers.patch \
    file://0008-mt76-mt7915-disable-mt7986-rx-hdr-trans-short.patch \
    file://1100-mt76-testmode-support-eeprom-handle.patch \
    file://1101-mt76-enable-more-5g-channels.patch \
    file://1102-mt76-testmode-add-attributes-for-setting-rf-config.patch \
    file://1103-mt76-mt7915-implement-config-set-in-testmode.patch \
    file://1104-mt76-testmode-add-attributes-to-support-off-channel-.patch \
    file://1105-mt76-mt7915-add-off-channel-scan-support-in-testmode.patch \
    file://1106-mt76-testmode-add-virtual-stations-support.patch \
    file://1107-mt76-testmode-support-to-dump-stats-from-different-v.patch \
    file://1108-mt76-testmode-rework-the-flow-of-init-tx-skb.patch \
    file://1109-mt76-testmode-add-support-to-queue-skb-of-multiple-s.patch \
    file://1110-mt76-mt7915-implement-aid-support-in-testmode.patch \
    file://1111-mt76-tool-add-more-commands.patch \
    file://2001-mt76-mt7915-add-L0.5-SER-for-mt7986.patch \
    "
