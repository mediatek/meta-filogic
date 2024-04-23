#!/bin/sh

#append DRIVERS "mac80211"

lookup_phy() {
	[ -n "$phy" ] && {
		[ -d /sys/class/ieee80211/$phy ] && return
	}

	local devpath
	config_get devpath "$device" path
	[ -n "$devpath" ] && {
		phy="$(iwinfo nl80211 phyname "path=$devpath")"
		[ -n "$phy" ] && return
	}

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	[ -n "$macaddr" ] && {
		for _phy in /sys/class/ieee80211/*; do
			[ -e "$_phy" ] || continue

			[ "$macaddr" = "$(cat ${_phy}/macaddress)" ] || continue
			phy="${_phy##*/}"
			return
		done
	}
	phy=
	return
}

find_mac80211_phy() {
	local device="$1"

	config_get phy "$device" phy
	lookup_phy
	[ -n "$phy" -a -d "/sys/class/ieee80211/$phy" ] || {
		echo "PHY for wifi device $1 not found"
		return 1
	}
	config_set "$device" phy "$phy"

	config_get macaddr "$device" macaddr
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/class/ieee80211/${phy}/macaddress)"
	}

	return 0
}

check_mac80211_device() {
	config_get phy "$1" phy
	[ -z "$phy" ] && {
		find_mac80211_phy "$1" >/dev/null || return 0
		config_get phy "$1" phy
	}
	[ "$phy" = "$dev" ] && found=1
}


__get_band_defaults() {
	local phy="$1"

	( iw phy "$phy" info; echo ) | awk '
BEGIN {
        bands = ""
}

($1 == "Band" || $1 == "") && band {
        if (channel) {
		mode="NOHT"
		if (ht) mode="HT20"
		if (vht && band != "1:") mode="VHT80"
		if (he) mode="HE80"
		if (he && band == "1:") mode="HE20"
		if (eht) mode="EHT80"
		if (eht && band == "1:") mode="EHT20"
                sub("\\[", "", channel)
                sub("\\]", "", channel)
                bands = bands band channel ":" mode " "
        }
        band=""
}

$1 == "Band" {
        band = $2
        channel = ""
	vht = ""
	ht = ""
	he = ""
	eht = ""
}

$0 ~ "Capabilities:" {
	ht=1
}

$0 ~ "VHT Capabilities" {
	vht=1
}

$0 ~ "HE Iftypes" {
	he=1
}

$0 ~ "EHT Iftypes" {
	eht=1
}

$1 == "*" && $3 == "MHz" && $0 !~ /disabled/ && band && !channel {
        channel = $4
}

END {
        print bands
}'
}

get_band_defaults() {
	local phy="$1"

	for c in $(__get_band_defaults "$phy"); do
		local band="${c%%:*}"
		c="${c#*:}"
		local chan="${c%%:*}"
		c="${c#*:}"
		local mode="${c%%:*}"

		case "$band" in
			1) band=2g;;
			2) band=5g;;
			3) band=60g;;
			4) band=6g;;
			*) band="";;
		esac

		[ -n "$band" ] || continue
		[ -n "$mode_band" -a "$band" = "6g" ] && return

		# hardcode for default band selection in MLO codebase
		[ "$phy" = "phy0" -a "$band" != "2g" ] && continue
		[ "$phy" = "phy1" -a "$band" != "5g" ] && continue
		[ "$phy" = "phy2" -a "$band" != "6g" ] && continue

		mode_band="$band"
		channel="$chan"
		htmode="$mode"
		if [ "$band" = "6g" ]
		then
			encryption=sae
			key=12345678
		else
			encryption=none
		fi
	done
}

detect_mac80211() {
	devidx=0
	#config_load wireless
	while :; do
		#config_get type "radio$devidx" type
		[ -n "$type" ] || break
		devidx=$(($devidx + 1))
	done

	if [ ! -f /etc/config/wireless ]; then
		mkdir -p /etc/config
		echo -n > /etc/config/wireless
	fi

	pcieCheck="$(uci get wireless.radio1.path)"
	checkpath="$(realpath /sys/class/ieee80211/phy1/device | cut -d/ -f4-)"

	if [[ "$pcieCheck"* != "$checkpath"* ]]; then
		echo -n > /etc/config/wireless
		rm /nvram/hostapd*
	fi

	old_path=""
	for _dev in /sys/class/ieee80211/*; do
		[ -e "$_dev" ] || continue

		dev="${_dev##*/}"

		found="$(uci get wireless.radio${devidx})"


		if [ "$found" == "wifi-device" ]; then
			devidx=$(($devidx + 1))
			continue
		fi

		mode_band=""
		channel=""
		htmode=""
		ht_capab=""
		

		get_band_defaults "$dev"

		path="$(realpath /sys/class/ieee80211/"$dev"/device | cut -d/ -f4-)"
		if [ -n "$path" ]; then
			if [ "$path" == "$old_path" ]; then
				dev_id="set wireless.radio${devidx}.path='$path'+1"
			else
				dev_id="set wireless.radio${devidx}.path='$path'"
			fi	
		else
			dev_id="set wireless.radio${devidx}.macaddr=$(cat /sys/class/ieee80211/${dev}/macaddress)"
		fi

		if [ "$(cat /sys/class/ieee80211/"$dev"/device/device)" == "0x7906" ]; then
			isMerlin=1
		fi

		if [ "$mode_band" = "6g" ]; then
			channel="37"
		fi
		setdisable="set wireless.radio${devidx}.disabled=0"


		if [ "$isMerlin" == "1" ] && [ "$devidx" == "2" ] && [ "$mode_band" == "2g" ]; then
			setdisable="set wireless.radio${devidx}.disabled=1"
		fi
	
		uci -q batch <<-EOF
			set wireless.radio${devidx}=wifi-device
			set wireless.radio${devidx}.type=mac80211
			${dev_id}
			set wireless.radio${devidx}.channel=${channel}
			set wireless.radio${devidx}.band=${mode_band}
			set wireless.radio${devidx}.htmode=$htmode
			${setdisable}
			set wireless.default_radio${devidx}=wifi-iface
			set wireless.default_radio${devidx}.device=radio${devidx}
			set wireless.default_radio${devidx}.network=lan
			set wireless.default_radio${devidx}.mode=ap
			set wireless.default_radio${devidx}.ssid=Filogic_${mode_band}
			set wireless.default_radio${devidx}.encryption=${encryption}
EOF
		[ -n "$key" ] && {
			uci -q set wireless.default_radio${devidx}.key=${key}
		}
		uci -q commit wireless

		devidx=$(($devidx + 1))
		old_path=$path
	done
}
detect_mac80211