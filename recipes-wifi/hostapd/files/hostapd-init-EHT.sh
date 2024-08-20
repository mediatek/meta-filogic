#!/bin/sh

mac80211_add_capabilities() {
	local __var="$1"; shift
	local __mask="$1"; shift
	local __out= oifs

	oifs="$IFS"
	IFS=:
	for capab in "$@"; do
		set -- $capab

		[ "$(($4))" -gt 0 ] || continue
		[ "$(($__mask & $2))" -eq "$((${3:-$2}))" ] || continue
		__out="$__out[$1]"
	done
	IFS="$oifs"

	export -n -- "$__var=$__out"
}

gen_vht_cap() {
    rxldpc=1
    short_gi_80=1
    short_gi_160=1
    tx_stbc_2by1=1
    su_beamformer=1
    su_beamformee=1
    mu_beamformer=1
    mu_beamformee=1
    vht_txop_ps=1
    htc_vht=1
    beamformee_antennas=5
    beamformer_antennas=4
    rx_antenna_pattern=1
    tx_antenna_pattern=1
    vht_max_a_mpdu_len_exp=7
    vht_max_mpdu=11454
    rx_stbc=4
    vht_link_adapt=3
    vht160=2

    vht_cap=0
    for cap in $(iw phy phy0 info | awk -F "[()]" '/VHT Capabilities/ { print $2 }'); do
        vht_cap="$(($vht_cap | $cap))"
    done

    cap_rx_stbc=$((($vht_cap >> 8) & 7))
    [ "$rx_stbc" -lt "$cap_rx_stbc" ] && cap_rx_stbc="$rx_stbc"
    vht_cap="$(( ($vht_cap & ~(0x700)) | ($cap_rx_stbc << 8) ))"

    mac80211_add_capabilities vht_capab $vht_cap \
        RXLDPC:0x10::$rxldpc \
        SHORT-GI-80:0x20::$short_gi_80 \
        SHORT-GI-160:0x40::$short_gi_160 \
        TX-STBC-2BY1:0x80::$tx_stbc_2by1 \
        SU-BEAMFORMER:0x800::$su_beamformer \
        SU-BEAMFORMEE:0x1000::$su_beamformee \
        MU-BEAMFORMER:0x80000::$mu_beamformer \
        MU-BEAMFORMEE:0x100000::$mu_beamformee \
        VHT-TXOP-PS:0x200000::$vht_txop_ps \
        HTC-VHT:0x400000::$htc_vht \
        RX-ANTENNA-PATTERN:0x10000000::$rx_antenna_pattern \
        TX-ANTENNA-PATTERN:0x20000000::$tx_antenna_pattern \
        RX-STBC-1:0x700:0x100:1 \
        RX-STBC-12:0x700:0x200:1 \
        RX-STBC-123:0x700:0x300:1 \
        RX-STBC-1234:0x700:0x400:1 \

    [ "$(($vht_cap & 0x800))" -gt 0 -a "$su_beamformer" -gt 0 ] && {
        cap_ant="$(( ( ($vht_cap >> 16) & 3 ) + 1 ))"
        [ "$cap_ant" -gt "$beamformer_antennas" ] && cap_ant="$beamformer_antennas"
        [ "$cap_ant" -gt 1 ] && vht_capab="$vht_capab[SOUNDING-DIMENSION-$cap_ant]"
    }

    [ "$(($vht_cap & 0x1000))" -gt 0 -a "$su_beamformee" -gt 0 ] && {
        cap_ant="$(( ( ($vht_cap >> 13) & 7 ) + 1 ))"
        [ "$cap_ant" -gt "$beamformee_antennas" ] && cap_ant="$beamformee_antennas"
        [ "$cap_ant" -gt 1 ] && vht_capab="$vht_capab[BF-ANTENNA-$cap_ant]"
    }
    # supported Channel widths
    vht160_hw=0
    [ "$(($vht_cap & 12))" -eq 4 -a 1 -le "$vht160" ] && \
        vht160_hw=1
    [ "$(($vht_cap & 12))" -eq 8 -a 2 -le "$vht160" ] && \
        vht160_hw=2
    [ "$vht160_hw" = 1 ] && vht_capab="$vht_capab[VHT160]"
    [ "$vht160_hw" = 2 ] && vht_capab="$vht_capab[VHT160-80PLUS80]"

    # maximum MPDU length
    vht_max_mpdu_hw=3895
    [ "$(($vht_cap & 3))" -ge 1 -a 7991 -le "$vht_max_mpdu" ] && \
        vht_max_mpdu_hw=7991
    [ "$(($vht_cap & 3))" -ge 2 -a 11454 -le "$vht_max_mpdu" ] && \
        vht_max_mpdu_hw=11454
    [ "$vht_max_mpdu_hw" != 3895 ] && \
        vht_capab="$vht_capab[MAX-MPDU-$vht_max_mpdu_hw]"

    # maximum A-MPDU length exponent
    vht_max_a_mpdu_len_exp_hw=0
    [ "$(($vht_cap & 58720256))" -ge 8388608 -a 1 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=1
    [ "$(($vht_cap & 58720256))" -ge 16777216 -a 2 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=2
    [ "$(($vht_cap & 58720256))" -ge 25165824 -a 3 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=3
    [ "$(($vht_cap & 58720256))" -ge 33554432 -a 4 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=4
    [ "$(($vht_cap & 58720256))" -ge 41943040 -a 5 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=5
    [ "$(($vht_cap & 58720256))" -ge 50331648 -a 6 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=6
    [ "$(($vht_cap & 58720256))" -ge 58720256 -a 7 -le "$vht_max_a_mpdu_len_exp" ] && \
        vht_max_a_mpdu_len_exp_hw=7
    vht_capab="$vht_capab[MAX-A-MPDU-LEN-EXP$vht_max_a_mpdu_len_exp_hw]"

    # whether or not the STA supports link adaptation using VHT variant
    vht_link_adapt_hw=0
    [ "$(($vht_cap & 201326592))" -ge 134217728 -a 2 -le "$vht_link_adapt" ] && \
        vht_link_adapt_hw=2
    [ "$(($vht_cap & 201326592))" -ge 201326592 -a 3 -le "$vht_link_adapt" ] && \
        vht_link_adapt_hw=3
    [ "$vht_link_adapt_hw" != 0 ] && \
        vht_capab="$vht_capab[VHT-LINK-ADAPT-$vht_link_adapt_hw]"

    echo vht_capab=$vht_capab >> /etc/hostapd-5G.conf
}

gen_he_6ghz_reg_pwr_type() { 
    local config_file="$1"

    if grep -q "^country_code=US" "$config_file"; then
	sed -i "/^he_6ghz_reg_pwr_type=.*/c\he_6ghz_reg_pwr_type=0" "$config_file"
	grep -q "^he_6ghz_reg_pwr_type" "$config_file" || echo "he_6ghz_reg_pwr_type=0" >> "$config_file"
    fi
}

create_hostapdConf() {
	devidx=0
	phyidx=0
        vap_per_radio=8
        radio_num="$(iw list | grep Wiphy | wc -l)"

        for _dev in /sys/class/ieee80211/*; do
		[ -e "$_dev" ] || continue

		dev="${_dev##*/}"

        band="$(uci get wireless.radio${phyidx}.band)"
        channel="$(uci get wireless.radio${phyidx}.channel)"
        # Use random MAC to prevent use the same MAC address
        rand="$(hexdump -C /dev/urandom | head -n 1 | awk '{printf ""$3":"$4""}' &)"
        killall hexdump
        MAC="00:0${devidx}:12:34:${rand}"
        chip="$(cat /sys/class/ieee80211/"$dev"/device/device)"

        if [ -e /sys/class/net/wlan$phyidx ]; then
            iw wlan$phyidx del > /dev/null
        elif [ -e /sys/class/net/wifi$phyidx ]; then
            for((i=0;i<$vap_per_radio;i++)); do
                ifidx=$(($phyidx+$i*$radio_num))
                ifname="$(cat /nvram/hostapd"$ifidx".conf | grep ^interface= | cut -d '=' -f2 | tr -d '\n')"
                if [ -n $ifname ]; then
                    hostapd_cli -i global raw REMOVE wifi$ifidx > /dev/null
                    if [ $i -eq 0 ]; then
                        iw wifi$ifidx del > /dev/null
                    fi
                fi
            done
        fi

	if [ "$(uci get wireless.radio${phyidx}.disabled)" == "1" ]; then
            phyidx=$(($phyidx + 1))
			continue
        fi

        if [ ! -f /nvram/hostapd"$devidx".conf ]; then
            touch /nvram/hostapd"$devidx".conf
        else
            for((i=0;i<$vap_per_radio;i++)); do
                ifidx=$(($phyidx+$i*$radio_num))
                ifname="$(cat /nvram/hostapd"$ifidx".conf | grep ^interface= | cut -d '=' -f2 | tr -d '\n')"
                vapstat="$(cat /nvram/vap-status | grep wifi"$ifidx"= | cut -d'=' -f2)"
                if [ -n $ifname ] && [[ $vapstat -eq "1" ]]; then
                    if [ $i = 0 ]; then
                        ## first interface in this phy
                        iw phy phy0 interface add $ifname type __ap > /dev/null
                    fi
                    touch /nvram/hostapd-acl$ifidx
                    touch /nvram/hostapd$ifidx.psk
                    touch /nvram/hostapd-deny$ifidx
                    if [ $phyidx = $ifidx ]; then
                        touch /tmp/phy0-wifi$ifidx
                    fi
                    hostapd_cli -i global raw ADD bss_config=$dev:/nvram/hostapd"$ifidx".conf
		fi
	    done
            devidx=$(($devidx + 1))
            phyidx=$(($phyidx + 1))
            continue
        fi


        if [ "$band" == "2g" ]; then
            cp -f /etc/hostapd-2G.conf /nvram/hostapd"$devidx".conf
        fi

        if [ "$band" == "5g" ]; then
            gen_vht_cap
            cp -f /etc/hostapd-5G.conf /nvram/hostapd"$devidx".conf
        fi

        if [ "$band" == "6g" ]; then
	    gen_he_6ghz_reg_pwr_type /etc/hostapd-6G.conf
            cp -f /etc/hostapd-6G.conf /nvram/hostapd"$devidx".conf
        fi

        sed -i "/^interface=.*/c\interface=wifi$devidx" /nvram/hostapd"$devidx".conf
        sed -i "/^bssid=/c\bssid=$MAC" /nvram/hostapd"$devidx".conf
        echo "wpa_psk_file=/nvram/hostapd$devidx.psk" >> /nvram/hostapd"$devidx".conf
        iw phy phy0 interface add wifi$devidx type __ap > /dev/null
        touch /nvram/hostapd-acl$devidx
        touch /nvram/hostapd$devidx.psk
        touch /nvram/hostapd-deny$devidx
        touch /tmp/phy0-wifi$devidx
        hostapd_cli -i global raw ADD bss_config=$dev:/nvram/hostapd"$devidx".conf && echo -e "wifi"$devidx"=1" >> /nvram/vap-status
        devidx=$(($devidx + 1))
        phyidx=$(($phyidx + 1))
		
	done
}
#Creating files for tracking AssociatedDevices
touch /tmp/AllAssociated_Devices_2G.txt
touch /tmp/AllAssociated_Devices_5G.txt

#Create wps pin request log file
touch /var/run/hostapd_wps_pin_requests.log


create_hostapdConf

exit 0
