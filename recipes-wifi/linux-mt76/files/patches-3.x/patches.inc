#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://0000-sync-to-master.patch \
    file://0001-Revert-wifi-mt76-mt7996-rely-on-mt76_connac_tx_compl.patch \
    file://0002-Revert-wifi-mt76-mt7996-rely-on-mt76_connac_txp_skb_.patch \
    file://0003-Revert-wifi-mt76-mt7996-rely-on-mt76_connac_txp_comm.patch \
    file://0004-wifi-mt76-mt7996-add-muru-support.patch \
    file://0005-wifi-mt76-mt7996-set-txd-v1.patch \
    file://0006-wifi-mt76-mt7996-add-802.11s-mesh-amsdu-de-amsdu-sup.patch \
    file://0007-mt76-revert-page-pool-changes.patch \
    file://0999-mt76-mt7996-for-build-pass.patch \
    "
