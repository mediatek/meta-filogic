#!/bin/sh

create_hostapdConf() {
	devidx=0
	

	for _dev in /sys/class/ieee80211/*; do
		[ -e "$_dev" ] || continue

		dev="${_dev##*/}"

		band="$(uci get wireless.radio${devidx}.band)"
		channel="$(uci get wireless.radio${devidx}.channel)"
        MAC="$(cat /sys/class/net/wlan${devidx}/address)"
        NEW_MAC=$(echo 0x$MAC| awk -F: '{printf "%02x:%s:%s:%s:%s:%s", strtonum($1)+2, $2, $3, $4 ,$5, $6}')
        chip="$(cat /sys/class/ieee80211/"$dev"/device/device)"

        if [ "$(uci get wireless.radio${devidx}.disabled)" == "1" ]; then
            devidx=$(($devidx + 1))
			continue
        fi

        if [ ! -f /nvram/hostapd"$devidx".conf ]; then
            touch /nvram/hostapd"$devidx".conf
        else
            iw dev wlan$devidx interface add wifi$devidx type __ap
            touch /tmp/hostapd-acl$devidx
            touch /tmp/hostapd$devidx.psk
            hostapd_cli -i global raw ADD bss_config=$dev:/nvram/hostapd"$devidx".conf 
            devidx=$(($devidx + 1))
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
        sed -i "/^bssid=/c\bssid=$NEW_MAC" /nvram/hostapd"$devidx".conf
        echo "wpa_psk_file=/tmp/hostapd$devidx.psk" >> /nvram/hostapd"$devidx".conf
        iw dev wlan$devidx interface add wifi$devidx type __ap
        touch /tmp/hostapd-acl$devidx
        touch /tmp/hostapd$devidx.psk
        hostapd_cli -i global raw ADD bss_config=$dev:/nvram/hostapd"$devidx".conf           
		devidx=$(($devidx + 1))
		
	done
}
echo -e "wifi0=1\nwifi1=1\nwifi2=0\nwifi3=0\nwifi4=0\nwifi5=0\nwifi6=0\nwifi7=0" >/tmp/vap-status
#Creating files for tracking AssociatedDevices
touch /tmp/AllAssociated_Devices_2G.txt
touch /tmp/AllAssociated_Devices_5G.txt

#Create wps pin request log file
touch /var/run/hostapd_wps_pin_requests.log


create_hostapdConf

exit 0