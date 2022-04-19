#patch subsys (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://110-mac80211_keep_keys_on_stop_ap.patch \
    file://120-cfg80211_allow_perm_addr_change.patch \
    file://150-disable_addr_notifier.patch \
    file://210-ap_scan.patch \
    file://301-mac80211-sta-randomize-BA-session-dialog-token-alloc.patch \
    file://303-mac80211-set-up-the-fwd_skb-dev-for-mesh-forwarding.patch \
    file://306-mac80211-use-coarse-boottime-for-airtime-fairness-co.patch \
    file://307-mac80211_hwsim-make-6-GHz-channels-usable.patch \
    file://308-mac80211-add-support-for-.ndo_fill_forward_path.patch \
    file://309-mac80211-minstrel_ht-fix-MINSTREL_FRAC-macro.patch \
    file://310-mac80211-minstrel_ht-reduce-fluctuations-in-rate-pro.patch \
    file://311-mac80211-minstrel_ht-rework-rate-downgrade-code-and-.patch \
    file://312-mac80211-split-beacon-retrieval-functions.patch \
    file://313-nl80211-MBSSID-and-EMA-support-in-AP-mode.patch \
    file://314-cfg80211-implement-APIs-for-dedicated-radar-detectio.patch \
    file://315-cfg80211-move-offchan_cac_event-to-a-dedicated-work.patch \
    file://316-cfg80211-fix-possible-NULL-pointer-dereference-in-cf.patch \
    file://317-cfg80211-schedule-offchan_cac_abort_wk-in-cfg80211_r.patch \
    file://318-cfg80211-allow-continuous-radar-monitoring-on-offcha.patch \
    file://319-mac80211-introduce-set_radar_offchan-callback.patch \
    file://320-cfg80211-rename-offchannel_chain-structs-to-backgrou.patch \
    file://323-mac80211-MBSSID-support-in-interface-handling.patch \
    file://324-mac80211-MBSSID-beacon-handling-in-AP-mode.patch \
    file://325-mac80211-MBSSID-channel-switch.patch \
    file://326-mac80211-update-bssid_indicator-in-ieee80211_assign_.patch \
    file://329-mac80211-minstrel_ht-fix-where-rate-stats-are-stored.patch \
    file://400-allow-ibss-mixed.patch \
    file://500-mac80211_configure_antenna_gain.patch \
    file://782-net-next-1-of-net-pass-the-dst-buffer-to-of_get_mac_address.patch \
    file://901-mac80211-check-twt-responder-when-setu-twt.patch \
    file://902-nl80211-internal-extend-CAC-time-for-weather-radar-c.patch \
    file://903-mac80211-it-s-invalid-case-when-frag_threshold-is-gr.patch \
    "
