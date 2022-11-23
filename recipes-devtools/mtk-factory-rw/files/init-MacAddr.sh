#!/bin/sh

lan_mac=`/usr/sbin/mtk_factory_rw.sh -r lan`
lan2_mac=`/usr/sbin/mtk_factory_rw.sh -r lan2`
wan_mac=`/usr/sbin/mtk_factory_rw.sh -r wan`


set_lan_wan_mac()
{
	NET_IF_LIST=`ls /sys/class/net`
	for vif in $NET_IF_LIST;
	do
		if [[ "$vif" == "eth1" ]] || [[ "$vif" == "lan"* ]]; then
			current_mac=`cat /sys/class/net/$vif/address`	
			if [[ "$lan_mac" != "ff:ff:ff:ff:ff:ff " ]] && [[ "$lan_mac" != "$current_mac " ]]; then
				ifconfig $vif down
				ifconfig $vif hw ether $lan_mac
			fi
		fi

		if [[ "$vif" == "eth3" ]]; then
			current_mac=`cat /sys/class/net/$vif/address`	
			if [[ "$lan_mac" != "ff:ff:ff:ff:ff:ff " ]] && [[ "$lan2_mac" != "$current_mac " ]]; then
				ifconfig $vif down
				ifconfig $vif hw ether $lan2_mac
			fi
		fi

		if [[ "$vif" == "eth2" ]] ; then
			current_wan_mac=`cat /sys/class/net/$vif/address`		
			if [[ "$wan_mac" != "ff:ff:ff:ff:ff:ff " ]] && [[ "$wan_mac" != "$current_wan_mac " ]]; then
				ifconfig $vif down
				ifconfig $vif hw ether $wan_mac
			fi
		fi
	done;
}

set_lan_wan_mac