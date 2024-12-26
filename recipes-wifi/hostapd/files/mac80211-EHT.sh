#!/bin/sh

ucode /usr/share/hostap/wifi-detect.uc
[ ! -f /etc/config/wireless ] && touch /etc/config/wireless
ucode /usr/share/hostap/mac80211.uc | uci -q batch