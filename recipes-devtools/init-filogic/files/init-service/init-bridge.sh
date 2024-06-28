#!/bin/sh
lan_ip=`syscfg get lan_ipaddr`
lan_mask=`syscfg get lan_netmask`
#Setting brlan0 bridge
if [ ! -d /sys/class/net/brlan0 ]
then
    brctl addbr brlan0
    ip link set brlan0 address `cat /sys/class/net/eth1/address`
    ifconfig brlan0 $lan_ip netmask $lan_mask up
fi

#Work around for Ethernet connected clients
if [ ! -d /sys/class/net/lan1 ]
then
brctl addif brlan0 eth1
else
brctl addif brlan0 lan0
brctl addif brlan0 lan1
brctl addif brlan0 lan2
brctl addif brlan0 lan3
brctl addif brlan0 lan4
brctl addif brlan0 lan5

ifconfig lan0 up
ifconfig lan1 up
ifconfig lan2 up
ifconfig lan3 up
ifconfig lan4 up
ifconfig lan5 up
fi
ifconfig eth1 up

board_name=`cat /proc/device-tree/model`

if [[ $board_name == *"7988"* ]]; then
    brctl addif brlan0 eth3
    ifconfig eth3 up
fi
#workaround: creating /opt/secure folder for ssh service
if [ ! -d /opt/secure ]; then
    mkdir -p /opt/secure
fi