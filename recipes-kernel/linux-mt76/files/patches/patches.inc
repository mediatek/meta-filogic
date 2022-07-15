#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0001-mt76-mt7915-rework-testmode-init-registers.patch \
    file://0002-mt76-testmode-rework-tx-antenna-setting.patch \
    file://0003-mt76-mt7915-rework-rx-testmode-stats.patch \
    file://0004-mt76-mt7915-fix-tx-descriptor.patch \
    file://0005-mt76-mt7915-drop-undefined-action-frame.patch \
    file://0008-mt76-common-RF-CR-idx-require-8-bits.patch \
    file://0010-mt76-mt7915-4addr-null-frame-using-fixed-rate-to-suc.patch \
    file://0011-mt76-testmode-use-random-payload-for-tx-packets.patch \
    file://0012-mt76-mt7915-add-spatial-extension-index-support.patch \
    file://100-Revert-of-net-pass-the-dst-buffer-to-of_get_mac_addr.patch \
    file://1001-mt76-mt7915-add-mtk-internal-debug-tools-for-mt76.patch \
    file://1002-mt76-mt7915-csi-implement-csi-support.patch \
    file://1003-mt76-mt7915-air-monitor-support.patch \
    file://1004-mt76-mt7915-add-support-for-muru_onoff-via-debugfs.patch \
    file://1005-mt76-certification-patches.patch \
    file://1006-mt76-mt7915-add-L0.5-system-error-recovery-support.patch \
    file://1007-mt76-mt7915-add-support-for-runtime-set-in-band-disc.patch \
    file://1008-mt76-mt7915-add-mt76-vendor-muru-onoff-command.patch \
    file://1111-mt76-testmode-additional-supports.patch \
    file://1112-mt76-mt7915-init-rssi-in-WTBL-when-add-station.patch \
    file://1113-mt76-mt7915-drop-packets-when-TWT-stations-use.patch \
    file://1114-mt76-airtime-fairness-feature-off-in-mac80211.patch \
    file://1115-mt76-mt7915-add-mt7986-and-mt7916-pre-calibration.patch \
    file://1116-mt76-mt7915-add-phy-capability-vendor-command.patch \
    file://1117-mt76-mt7915-add-vendor-subcmd-EDCCA-ctrl.patch \
    file://1118-mt76-mt7915-implement-bin-file-mode.patch \
    file://1119-mt76-mt7915-initialize-wcid.patch \
    file://3001-mt76-add-wed-tx-support.patch \
    file://3002-mt76-add-wed-rx-support.patch \
    file://3003-mt76-add-fill-receive-path-to-report-wed-idx.patch \
    "
