#patch subsys (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://110-mac80211_keep_keys_on_stop_ap.patch \
    file://120-cfg80211_allow_perm_addr_change.patch \
    file://210-ap_scan.patch \
    file://301-mac80211-sta-randomize-BA-session-dialog-token-alloc.patch \
    file://302-mac80211-minstrel_ht-fix-MINSTREL_FRAC-macro.patch \
    file://303-mac80211-minstrel_ht-reduce-fluctuations-in-rate-pro.patch \
    file://304-mac80211-minstrel_ht-rework-rate-downgrade-code-and-.patch \
    file://305-mac80211-increase-quantum-for-airtime-scheduler.patch \
    file://306-01-wifi-mac80211-add-internal-handler-for-wake_tx_queue.patch \
    file://306-02-wifi-mac80211-add-wake_tx_queue-callback-to-drivers.patch \
    file://306-03-wifi-mac80211-Drop-support-for-TX-push-path.patch \
    file://306-04-wifi-realtek-remove-duplicated-wake_tx_queue.patch \
    file://310-mac80211-add-support-for-restricting-netdev-features.patch \
    file://400-allow-ibss-mixed.patch \
    file://500-mac80211_configure_antenna_gain.patch \
    file://782-net-next-1-of-net-pass-the-dst-buffer-to-of_get_mac_address.patch \
    file://800-mac80211-mask-nested-A-MSDU-support-for-mesh.patch \
    file://900-mac80211-mtk-do-not-setup-twt-when-twt-responder-is-.patch \
    file://901-nl80211-mtk-extend-CAC-time-for-weather-radar-channe.patch \
    file://902-mac80211-mtk-it-s-invalid-case-when-frag_threshold-i.patch \
    file://903-mac80211-mtk-airtime_flags-depends-on-NL80211_EXT_FE.patch \
    file://904-mac80211-mtk-add-support-for-runtime-set-inband-disc.patch \
    file://905-mac80211-mtk-remove-timerout-handle-for-ax210-iot-is.patch \
    file://906-cfg80211-mtk-implement-DFS-status-show-cac-and-nop-s.patch \
    file://907-mac80211-mtk-Set-TWT-Information-Frame-Disabled-bit-.patch \
    file://908-mac80211-mtk-fix-the-issue-of-AP-and-STA-starting-on.patch \
    file://99900-mac80211-mtk-add-fill-receive-path-ops-to-get-wed-i.patch \
    file://99901-fix-build-failed.patch \
    "
