#!/bin/sh

create_hostapdConf() {
	devidx=0
	phyidx=0
	old_path=""
	pcie7915count=0
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

        if [ $chip == "0x7915" ]; then
            path="$(realpath /sys/class/ieee80211/"$dev"/device | cut -d/ -f4-)"
            if [ -n "$path" ]; then
                if [ "$path" == "$old_path" ] || [ "$old_path" == "" ]; then
                    pcie7915count="1"
                else
                    pcie7915count="2"    
                fi
            fi
            old_path=$path
        fi

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
                        iw phy phy$phyidx interface add $ifname type __ap > /dev/null
                    fi
                    touch /nvram/hostapd-acl$ifidx
                    touch /nvram/hostapd$ifidx.psk
                    touch /nvram/hostapd-deny$ifidx
                    if [ $phyidx = $ifidx ]; then
                        touch /tmp/$dev-wifi$ifidx
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

            if [ $chip == "0x7906" ]; then
                cp -f /etc/hostapd-5G-7916.conf /nvram/hostapd"$devidx".conf
            elif [ $chip == "0x7915" ]; then
                cp -f /etc/hostapd-5G-7915.conf /nvram/hostapd"$devidx".conf
            else
                cp -f /etc/hostapd-5G.conf /nvram/hostapd"$devidx".conf
            fi                 
        fi

        if [ "$band" == "6g" ]; then
            cp -f /etc/hostapd-6G.conf /nvram/hostapd"$devidx".conf
        fi

	    sed -i "/^interface=.*/c\interface=wifi$devidx" /nvram/hostapd"$devidx".conf
        sed -i "/^bssid=/c\bssid=$MAC" /nvram/hostapd"$devidx".conf
        echo "wpa_psk_file=/nvram/hostapd$devidx.psk" >> /nvram/hostapd"$devidx".conf
        iw phy phy$phyidx interface add wifi$devidx type __ap > /dev/null
        touch /nvram/hostapd-acl$devidx
        touch /nvram/hostapd$devidx.psk
        touch /nvram/hostapd-deny$devidx
        touch /tmp/$dev-wifi$devidx
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
