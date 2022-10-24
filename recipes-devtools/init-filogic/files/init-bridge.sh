#!/bin/sh

#Setting brlan0 bridge
if [ ! -d /sys/class/net/brlan0 ]
then
    brctl addbr brlan0
    ip link set brlan0 address `cat /sys/class/net/eth1/address`
    ifconfig brlan0 10.0.0.1 netmask 255.255.255.0 up
fi

#Work around for Ethernet connected clients
if [ ! -d /sys/class/net/lan0 ]
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


#workaround: creating /opt/secure folder for ssh service
if [ ! -d /opt/secure ]; then
    mkdir /opt/secure
fi